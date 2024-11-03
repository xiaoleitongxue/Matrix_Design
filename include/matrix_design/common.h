#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <type_traits>

template <bool B, typename T> using Enable_if = std::enable_if_t<B, T>;
template <typename From, typename To>
using Convertible = typename std::is_convertible<From, To>::type;
template <typename T, typename U>
using Same = typename std::is_same<T, U>::type;

namespace matrix_impl {

// N > 1
template <typename T, std::size_t N> struct Matrix_init {
  using type = std::initializer_list<typename Matrix_init<T, N - 1>::type>;
};
// N = 1
template <typename T> struct Matrix_init<T, 1> {
  using type = std::initializer_list<T>;
};
// N = 0
template <typename T> struct Matrix_init<T, 0>;

template <typename List> auto check_non_jagged(const List &list) -> bool {
  auto index = list.begin();
  for (auto j = index + 1; j != list.end(); ++j) {
    if (index->size() != j->size()) {
      return false;
    }
  }
  return true;
}

template <std::size_t N, typename I, typename List>
auto add_extents(I &first, const List &list)
    -> std::enable_if_t<(N == 1), void> {
  *first++ = list.size();
}

template <std::size_t N, typename I, typename List>
auto add_extents(I &first, const List &list)
    -> std::enable_if_t<(N > 1), void> {
  assert(check_non_jagged(list));
  *first = list.size();
  add_extents<N - 1>(++first, *list.begin());
}

template <std::size_t N, typename List>
auto derive_extents(const List &list) -> std::array<std::size_t, N> {
  std::array<std::size_t, N> array;
  auto elem = array.begin();
  add_extents<N>(elem, list);
  return array;
}

// When we reach a list with non-initializer_list elements, we insert those
// elements into our vector
template <typename T, typename Vec>
void add_list(const T *first, const T *last, Vec &vec) {
  vec.insert(vec.end(), first, last);
}

template <typename T, typename Vec>
void add_list(const std::initializer_list<T> *first,
              const std::initializer_list<T> *last, Vec &vec) {
  for (; first != last; ++first) {
    add_list(first->begin(), first->end(), vec);
  }
}

template <typename T, typename Vec>
void insert_flat(std::initializer_list<T> list, Vec &vec) {
  add_list(list.begin(), list.end(), vec);
}

template <typename T, std::size_t N, typename Vec>
auto insert_from_m_r(T *first, std::array<std::size_t, N> extents,
                     std::array<std::size_t, N> strides, Vec &vec)
    -> std::enable_if_t<(N == 1), void> {
  T *last = first + extents[0];
  vec.insert(vec.end(), first, last);
}

template <typename T, std::size_t N, typename Vec>
auto insert_from_m_r(T *first, std::array<std::size_t, N> extents,
                     std::array<std::size_t, N> strides, Vec &vec)
    -> std::enable_if_t<(N > 1), void> {
  std::size_t extent = extents[0];
  std::size_t stride = strides[0];
  std::array<std::size_t, N - 1> extents_;
  std::array<std::size_t, N - 1> strides_;
  std::copy(extents.begin() + 1, extents.end(), extents_.begin());
  std::copy(strides.begin() + 1, strides.end(), strides_.begin());
  for (std::size_t i = 0; i < extent; ++i) {
    T *offset = first + i * stride;
    insert_from_m_r<T, N - 1, Vec>(offset, extents_, strides_, vec);
  }
}

template <std::size_t N, typename Array>
auto computing_stride(const Array &extents) -> std::array<std::size_t, N> {
  std::array<std::size_t, N> strides;
  for (std::size_t i = 0; i < N; ++i) {
    strides[i] = 1;
  }
  for (std::size_t i = 0; i < N - 1; ++i) {
    std::size_t product = 1;
    for (std::size_t j = i + 1; j < N; ++j) {
      product *= extents[j];
    }
    strides[i] = product;
  }
  return strides;
}

template <std::size_t N, typename... Dims, typename Array>
auto check_bounds(const Array &extents, Dims... dims) -> bool {
  std::array<std::size_t, N> indexes{std::size_t(dims)...};
  return std::equal(indexes.begin(), indexes.begin() + N, extents.begin(),
                    std::less<std::size_t>{});
}

constexpr auto All() -> bool { return true; }

template <typename... Args>
constexpr auto All(bool left, Args... args) -> bool {
  return left && All(args...);
}

template <typename... Args> constexpr auto Requesting_element() -> bool {
  return All(std::is_convertible<Args, std::size_t>()...);
}

constexpr auto Some() -> bool { return true; }

template <typename... Args>
constexpr auto Some(bool left, Args... args) -> bool {
  return left || All(args...);
}

template <std::size_t N, typename Array>
auto computing_size(const Array &extents) -> std::size_t {
  std::size_t size = 1;
  for (int i = 0; i < N; ++i) {
    size *= extents[i];
  }
  return size;
}

} // namespace matrix_impl

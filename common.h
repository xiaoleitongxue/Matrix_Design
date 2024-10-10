#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <initializer_list>

#include <type_traits>

#include <cassert>

template <bool B, typename T>
using Enable_if = typename std::enable_if<B, T>::type;
template <typename From, typename To>
using Convertible = typename std::is_convertible<From, To>::type;
template <typename T, typename U>
using Same = typename std::is_same<T, U>::type;

namespace Matrix_impl {

// N > 1
template <typename T, size_t N>
struct Matrix_init {
    using type = std::initializer_list<typename Matrix_init<T, N - 1>::type>;
};
// N = 1
template <typename T>
struct Matrix_init<T, 1> {
    using type = std::initializer_list<T>;
};
// N = 0
template <typename T>
struct Matrix_init<T, 0>;

template <typename List>
bool check_non_jagged(const List& list)
{
    auto i = list.begin();
    for (auto j = i + 1; j != list.end(); ++j) {
        if (i->size() != j->size()) {
            return false;
        }
    }
    return true;
}

template <size_t N, typename I, typename List>
typename std::enable_if<(N == 1), void>::type add_extents(I& first,
    const List& list)
{
    *first++ = list.size();
}

template <size_t N, typename I, typename List>
typename std::enable_if<(N > 1), void>::type add_extents(I& first,
    const List& list)
{
    assert(check_non_jagged(list));
    *first = list.size();
    add_extents<N - 1>(++first, *list.begin());
}

template <size_t N, typename List>
std::array<size_t, N> derive_extents(const List& list)
{
    std::array<size_t, N> a;
    auto f = a.begin();
    add_extents<N>(f, list);
    return a;
}

// When we reach a list with non-initializer_list elements, we insert those
// elements into our vector
template <typename T, typename Vec>
void add_list(const T* first, const T* last, Vec& vec)
{
    vec.insert(vec.end(), first, last);
}

template <typename T, typename Vec>
void add_list(const std::initializer_list<T>* first,
    const std::initializer_list<T>* last, Vec& vec)
{
    for (; first != last; ++first) {
        add_list(first->begin(), first->end(), vec);
    }
}

template <typename T, typename Vec>
void insert_flat(std::initializer_list<T> list, Vec& vec)
{
    add_list(list.begin(), list.end(), vec);
}

template <typename T, size_t N, typename Vec>
typename std::enable_if<(N == 1), void>::type insert_from_m_r(T* first, std::array<size_t, N> extents, std::array<size_t, N> strides, Vec& vec)
{
    T* last = first + extents[0];
    vec.insert(vec.end(), first, last);
}

template <typename T, size_t N, typename Vec>
typename std::enable_if<(N > 1), void>::type insert_from_m_r(T* first, std::array<size_t, N> extents, std::array<size_t, N> strides, Vec& vec)
{
    size_t extent = extents[0];
    size_t stride = strides[0];
    std::array<size_t, N - 1> extents_;
    std::array<size_t, N - 1> strides_;
    std::copy(extents.begin() + 1, extents.end(), extents_.begin());
    std::copy(strides.begin() + 1, strides.end(), strides_.begin());
    for (size_t i = 0; i < extent; ++i) {
        T* offset = first + i * stride;
        insert_from_m_r<T, N - 1, Vec>(offset, extents_, strides_, vec);
    }
}

template <size_t N, typename Array>
std::array<size_t, N> computing_stride(const Array& extents)
{
    std::array<size_t, N> strides;
    for (size_t i = 0; i < N; ++i) {
        strides[i] = 1;
    }
    for (size_t i = 0; i < N - 1; ++i) {
        size_t product = 1;
        for (size_t j = i + 1; j < N; ++j) {
            product *= extents[j];
        }
        strides[i] = product;
    }
    return strides;
}

template <size_t N, typename... Dims, typename Array>
bool check_bounds(const Array& extents, Dims... dims)
{
    size_t indexes[N] { size_t(dims)... };
    return std::equal(indexes, indexes + N, extents.begin(), std::less<size_t> {});
}

constexpr bool All() { return true; }

template <typename... Args>
constexpr bool All(bool b, Args... args)
{
    return b && All(args...);
}

template <typename... Args>
constexpr bool Requesting_element()
{
    return All(std::is_convertible<Args, size_t>()...);
}

constexpr bool Some() { return true; }

template <typename... Args>
constexpr bool Some(bool b, Args... args)
{
    return b || All(args...);
}

template <size_t N, typename Array>
size_t computing_size(const Array& extents)
{
    size_t size = 1;
    for (int i = 0; i < N; ++i) {
        size *= extents[i];
    }
    return size;
}

} // namespace Matrix_impl

#pragma once

#include "common.h"
#include <array>
#include <cstddef>
#include <initializer_list>
#include <numeric>

template <std::size_t N> struct Matrix_slice;

struct Slice {
  std::size_t i;
  std::size_t j{};

  Slice(const std::size_t index_0, const std::size_t index_1)
      : i(index_0), j(index_1) {}

  explicit Slice(const std::size_t index) : i(index) {}
};

template <std::size_t N> struct Matrix_slice {
  Matrix_slice() = default; // empty matrix
  Matrix_slice(const Matrix_slice &) = default;
  auto operator=(const Matrix_slice &) -> Matrix_slice & = default;
  Matrix_slice(Matrix_slice &&) = default;
  auto operator=(Matrix_slice &&) -> Matrix_slice & = default;

  Matrix_slice(std::size_t start,
               std::initializer_list<std::size_t> extents); // extents
  Matrix_slice(std::size_t start, std::initializer_list<std::size_t> extents,
               std::initializer_list<std::size_t> strides);

  template <typename... Dims> explicit Matrix_slice(Dims... dims); // N extents

  template <typename... Dims, typename = std::enable_if<matrix_impl::All(
                                  std::is_convertible<Dims, std::size_t>()...)>>
  // calculate index from                                                   //
  // a set of subscripts
  auto operator()(Dims... dims) const -> std::size_t;

  std::size_t size{};                 // total number of elements
  std::size_t start{};                // starting offset
  std::array<std::size_t, N> extents; // number of elements in each dimension
  std::array<std::size_t, N>
      strides; // offsets between elements in each dimension
};

template <std::size_t N>
template <typename... Dims>
inline Matrix_slice<N>::Matrix_slice(Dims... dims)
    : extents{static_cast<std::size_t>(dims)...}, strides{N} {
  static_assert(sizeof...(Dims) == N, "Dims must be N");
  size = (dims * ...);
  strides = matrix_impl::computing_stride<N>(extents);
  start = static_cast<std::size_t>(0);
}

template <std::size_t N>
template <typename... Dims, typename>
inline auto Matrix_slice<N>::operator()(Dims... dims) const -> std::size_t {
  static_assert(sizeof...(Dims) == N, "Number of params must be N");
  std::array<std::size_t, N> args{
      static_cast<std::size_t>(dims)...}; // copy arguments into an array
  return std::inner_product(args.begin(), args.begin() + N, strides.begin(),
                            static_cast<std::size_t>(0));
}

template <std::size_t N>
inline Matrix_slice<N>::Matrix_slice(const std::size_t start,
                                     std::initializer_list<std::size_t> extents)
    : start{start}, extents{extents} {
  static_assert(extents.size() == N, "Dims must be N");

  size = matrix_impl::computing_size<N>(extents);

  strides = matrix_impl::computing_stride<N>(extents);
}

template <std::size_t N>
inline Matrix_slice<N>::Matrix_slice(const std::size_t start,
                                     std::initializer_list<std::size_t> extents,
                                     std::initializer_list<std::size_t> strides)
    : start{start}, extents{extents}, strides{strides} {
  static_assert(extents.size() == N, "Extents size must be N");
  static_assert(strides.size() == N, "Extents size must be N");
  size = matrix_impl::computing_size<N>(extents);
}

template <typename... Args> constexpr auto Requesting_slice() -> bool {
  return matrix_impl::All(
             (Convertible<Args, std::size_t>() || Same<Args, Slice>())...) &&
         matrix_impl::Some(Same<Args, Slice>()...);
}

template <std::size_t M, std::size_t N, typename T>
auto do_slice_dim(const Matrix_slice<N> &os, Matrix_slice<N> &ns, const T &s)
    -> std::size_t {
  std::size_t i = 0;
  std::size_t j = 0;
  if constexpr (std::is_same<T, std::size_t>()) {
    i = s;
    j = i + 1;
  } else if constexpr (std::is_same<T, Slice>()) {
    const auto s_ = static_cast<Slice>(s);
    i = s_.i;
    j = s_.j;
  }

  std::size_t current_dim = os.extents.size() - M;

  std::size_t extent = j - i;
  std::size_t stride = os.strides[current_dim];

  ns.extents[current_dim] = extent;
  ns.strides[current_dim] = stride;

  const std::size_t start = i * os.strides[current_dim];
  return start;
}

template <std::size_t N, typename T, typename... Args>
auto do_slice(const Matrix_slice<N> &os, Matrix_slice<N> &ns, const T &s,
              const Args &...args) -> std::size_t {
  // do slice with slice s
  std::size_t m = do_slice_dim<sizeof...(Args) + 1>(os, ns, s);
  std::size_t n = do_slice(os, ns, args...);
  // m+n is offset
  return m + n;
}

// when args is empty
template <std::size_t N>
auto do_slice(const Matrix_slice<N> &os, Matrix_slice<N> &ns) -> std::size_t {
  return 0;
}

// slice dim 0;
template <std::size_t dim, typename T, std::size_t N>
auto slice_dim(std::size_t n, Matrix_slice<N> &desc, Matrix_slice<N - 1> &row)
    -> Enable_if<(dim == 0), void> {
  row.start = n * desc.strides[0];
  std::copy(desc.extents.begin() + 1, desc.extents.end(), row.extents.begin());
  std::copy(desc.strides.begin() + 1, desc.strides.end(), row.strides.begin());
  row.size = matrix_impl::computing_size<N - 1>(row.extents);
}

// slice dim 1
template <std::size_t dim, typename T, std::size_t N>
auto slice_dim(std::size_t n, Matrix_slice<N> &desc, Matrix_slice<N - 1> &col)
    -> Enable_if<(dim == 1), void> {
  col.start = n * desc.strides[1];
  std::size_t j = 0;
  for (std::size_t i = 0; i < N; ++i) {
    if (i == 1) {
      continue;
    }
    col.extents[j++] = desc.extents[i];
    col.strides[j++] = desc.strides[i];
  }
  col.size = matrix_impl::computing_size<N - 1>(col.extents);
}

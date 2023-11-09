#pragma once
#include "Matrix_slice.h"
#include "common.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <ostream>
#include <type_traits>
#include <vcruntime.h>

template <size_t N> struct Matrix_slice;
template <> struct Matrix_slice<1>;
template <> struct Matrix_slice<2>;
template <size_t N, typename Array>
std::array<size_t, N> computing_stride(const Array &extents);
template <size_t N, typename Array> size_t computing_size(const Array &extents);
template <size_t N, typename... Dims>
bool check_bounds(const Matrix_slice<N> &slice, Dims... dims);








template <size_t N, typename Array>
std::array<size_t, N> computing_stride(const Array &extents) {
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

template <size_t N, typename... Dims>
bool check_bounds(const Matrix_slice<N> &slice, Dims... dims) {
  size_t indexes[N]{size_t(dims)...};
  return std::equal(indexes, indexes + N, slice.extents, std::less<size_t>{});
}

constexpr bool All() { return true; }

template <typename... Args> constexpr bool All(bool b, Args... args) {
  return b && All(args...);
}

template <typename... Args> constexpr bool Requesting_element() {
  return All(std::is_convertible<Args, size_t>()...);
}

constexpr bool Some(){return true;}

template <typename... Args> constexpr bool Some(bool b, Args... args) {
  return b || All(args...);
}



template <typename... Args> constexpr bool Requesting_slice() {
  return All((Convertible<Args, size_t>() || Same<Args,Matrix_slice<sizeof...(Args)>>())...) && Some(Same<Args,Matrix_slice<sizeof...(Args)>>()...);;
}


template<size_t N>
Enable_if<(N > 1), size_t> do_slice_dim(){

}


template<size_t N>
Enable_if<(N == 1), size_t> do_slice_dim(){
  
}


template <size_t N, typename T, typename... Args>
size_t do_slice(const Matrix_slice<N> &os, Matrix_slice<N> &ns, const T &s,
                const Args &...args) {
  // do slice at dim s
  size_t m = do_slice_dim<sizeof...(Args) + 1>(os, ns, s);
  size_t n = do_slice(os, ns, args...);
  return m + n;
}

template <size_t N>
size_t do_slice(const Matrix_slice<N> &os, Matrix_slice<N> &ns) {
  return 0;
}

// slice dim 0;
template <size_t dim, typename T, size_t N>
Enable_if<(dim == 0), void> slice_dim(size_t n, Matrix_slice<N> &desc, Matrix_slice<N> &row) {
  row.start = n * desc.strides[0];
  std::copy(desc.extents.begin() + 1, desc.extents.end(), row.extents);
  std::copy(desc.strides.begin() + 1, desc.strides.end(), row.strides);
  row.size = computing_size<N - 1>(row.extents);
}

//slice dim 1
template <size_t dim, typename T, size_t N>
Enable_if<(dim == 1), void> slice_dim(size_t n, Matrix_slice<N> &desc, Matrix_slice<N> &col) {
  col.start = n * desc.strides[1];
  size_t j = 0;
  for(size_t i = 0; i < N; ++i){
    if(i == 1){
      continue;
    }
    col.extents[j++] = desc.extents[i];
    col.strides[j++] = desc.strides[i];
  }
  col.size = computing_size<N - 1>(col.extents);
}

template <size_t N, typename Array>
size_t computing_size(const Array &extents) {
  size_t size = 1;
  for (int i = 0; i < N; ++i) {
    size *= extents[i];
  }
  return size;
}

template <size_t N> struct Matrix_slice {
  Matrix_slice() = default; // empty matrix
  Matrix_slice(size_t s, std::initializer_list<size_t> exts);
  Matrix_slice(size_t s, std::initializer_list<size_t> exts,
               std::initializer_list<size_t> strs);
  template <typename... Dims> Matrix_slice(Dims... dims);
  template <typename... Dims, typename = std::enable_if<
                                  All(std::is_convertible<Dims, size_t>()...)>>
  size_t operator()(Dims... dims) const;
  size_t size;                   // total number of elements
  size_t start;                  // starting offset
  std::array<size_t, N> extents; // number of elemens in each dimension
  std::array<size_t, N> strides; // offsets between elements in each dimension
};

template <> struct Matrix_slice<1> {
  Matrix_slice() = default; // empty matrix
  Matrix_slice(size_t s, std::initializer_list<size_t> exts);
  Matrix_slice(size_t s, std::initializer_list<size_t> exts,
               std::initializer_list<size_t> strs);
  template <typename... Dims>
  Matrix_slice(Dims... dims) : extents{size_t(dims)...} {
    static_assert(sizeof...(Dims) == 1, "");
    size = (dims * ... * 1);
    strides = computing_stride<1>(extents);
    start = size_t(0);
  }
  template <typename... Dims, typename = std::enable_if<
                                  All(std::is_convertible<Dims, size_t>()...)>>
  size_t operator()(size_t i) const {
    return i;
  };
  size_t size;                   // total number of elements
  size_t start;                  // starting offset
  std::array<size_t, 1> extents; // number of elemens in each dimension
  std::array<size_t, 1> strides; // offsets between elements in each dimension
};

template <> struct Matrix_slice<2> {
  Matrix_slice() = default; // empty matrix
  Matrix_slice(size_t s, std::initializer_list<size_t> exts);
  Matrix_slice(size_t s, std::initializer_list<size_t> exts,
               std::initializer_list<size_t> strs);
  template <typename... Dims>
  Matrix_slice(Dims... dims) : extents{size_t(dims)...} {
    static_assert(sizeof...(Dims) == 2, "");
    size = (dims * ...);
    strides = computing_stride<2>(extents);
    start = size_t(0);
  }
  template <typename... Dims, typename = std::enable_if<
                                  All(std::is_convertible<Dims, size_t>()...)>>
  size_t operator()(size_t i, size_t j) const {
    return i * strides[0] + j;
  };
  size_t size;                   // total number of elements
  size_t start;                  // starting offset
  std::array<size_t, 2> extents; // number of elemens in each dimension
  std::array<size_t, 2> strides; // offsets between elements in each dimension
};

template <size_t N>
template <typename... Dims>
inline Matrix_slice<N>::Matrix_slice(Dims... dims)
    : extents{size_t(dims)...}, strides{N} {
  static_assert(sizeof...(Dims) == N, "");
  size = (dims * ...);
  strides = computing_stride<N>(extents);
  start = size_t(0);
}

template <size_t N>
template <typename... Dims, typename>
inline size_t Matrix_slice<N>::operator()(Dims... dims) const {
  static_assert(sizeof...(Dims) == N, "");
  size_t args[N]{size_t(dims)...}; // copy arguments into an array
  return std::inner_product(args, args + N, strides.begin(), size_t(0));
}

template <size_t N>
inline Matrix_slice<N>::Matrix_slice(size_t s,
                                     std::initializer_list<size_t> exts)
    : extents{exts}, start{s} {
  static_assert(exts.size() == N, "");

  size = computing_size<N>(extents);

  strides = computing_stride<N>(extents);
}

template <size_t N>
inline Matrix_slice<N>::Matrix_slice(size_t s,
                                     std::initializer_list<size_t> exts,
                                     std::initializer_list<size_t> strs)
    : extents{exts}, strides{strs}, start{s} {
  static_assert(exts.size() == N, "");
  size = computing_stride<N>(extents);
}
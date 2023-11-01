// Matrix_Design.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <ostream>
#include <type_traits>
#include <vcruntime.h>

// decaler class prototype
template <size_t N> struct Matrix_slice;
template <typename T, size_t N> class Matrix_base;
template <typename T, size_t N> class Matrix_ref;
template <typename T, size_t N> class Matrix;

template <bool B, typename T>
using Enable_if = typename std::enable_if<B, T>::type;

namespace Matrix_impl {
// N > 1
template <typename T, size_t N> struct Matrix_init {
  using type = std::initializer_list<typename Matrix_init<T, N - 1>::type>;
};
// N = 1
template <typename T> struct Matrix_init<T, 1> {
  using type = std::initializer_list<T>;
};
// N = 0
template <typename T> struct Matrix_init<T, 0>;

template <typename List> bool check_non_jagged(const List &list) {
  auto i = list.begin();
  for (auto j = i + 1; j != list.end(); ++j) {
    if (i->size() != j->size()) {
      return false;
    }
  }
  return true;
}

template <size_t N, typename I, typename List>
typename std::enable_if<(N == 1), void>::type add_extents(I &first,
                                                          const List &list) {
  *first++ = list.size();
}

template <size_t N, typename I, typename List>
typename std::enable_if<(N > 1), void>::type add_extents(I &first,
                                                         const List &list) {
  assert(check_non_jagged(list));
  *first = list.size();
  add_extents<N - 1>(++first, *list.begin());
}

template <size_t N, typename List>
std::array<size_t, N> derive_extents(const List &list) {
  std::array<size_t, N> a;
  auto f = a.begin();
  add_extents<N>(f, list);
  return a;
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

template <size_t N, typename Array>
std::array<size_t, N> computing_stride(const Array &extents) {
  std::array<size_t, N> strides;

  // init all stride to 1
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

template <size_t N, typename Array>
size_t computing_size(const Array &extents) {
  size_t size = 1;
  for (int i = 0; i < N; ++i) {
    size *= extents[i];
  }
  return size;
}

template <size_t N, typename... Dims>
bool check_bounds(const Matrix_slice<N> &slice, Dims... dims) {
  size_t indexes[N]{size_t(dims)...};
  return std::equal(indexes, indexes + N, slice.extents, std::less<size_t>{});
}
constexpr bool All() { return true; }

template <typename... Args> constexpr bool All(bool b, Args... args) {
  return b && All(args);
}

template <typename... Args> constexpr bool Requesting_element() {
  return All(std::is_convertible<Args, size_t>()...);
}




template<size_t N, typename T, typename... Args>
size_t do_slice(const Matrix_slice<N>& os, Matrix_slice<N>& ns, const T& s, const Args&... args){
  size_t m = do_slice_dim<sizeof...(Args) + 1>(os, ns, s);
  size_t n = do_slice(os, ns, args...);
  return m + n;
}

template<size_t N>
size_t do_slice(const Matrix_slice<N> &os, Matrix_slice<N> &ns){
  return 0;
}

} // namespace Matrix_impl

template <typename T, size_t N>
using Matrix_initializer = typename Matrix_impl::Matrix_init<T, N>::type;

// TODO: Reference additional headers your program requires here.
#include <numeric>
#include <vector>

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

template <typename T, size_t N> class Matrix_base {
  // common stuff
public:
  using value_type = T;
  using iterator = typename std::vector<T>::iterator;
  using const_iterator = typename std::vector<T>::const_iterator;
};

// A Matrix_ref simply points to the elements of its Matrix
template <typename T, size_t N> class Matrix_ref : public Matrix_base<T, N> {
  // special to matrix_ref
public:
  // default constructor
  Matrix_ref() = default;
  Matrix_ref(Matrix_ref &&) = default;                 // move constructor
  Matrix_ref &operator=(Matrix_ref &&) = default;      // move assignment
  Matrix_ref(Matrix_ref const &) = default;            // copy constructor
  Matrix_ref &operator=(Matrix_ref const &) = default; // copy assignment
  ~Matrix_ref() = default;

  Matrix_ref(const Matrix_slice<N> &s, T *p) : desc{s}, ptr{p} {}

private:
  Matrix_slice<N> desc; // the shape of matrix
  T *ptr;               // the first element of its matrix
};

template <typename T, size_t N> class Matrix : public Matrix_base<T, N> {
  // special to matrix
public:
  static constexpr size_t order = N; // dimensions
  Matrix() = default;
  Matrix(Matrix &&) = default;                 // move constructor
  Matrix &operator=(Matrix &&) = default;      // move assignment
  Matrix(Matrix const &) = default;            // copy constructor
  Matrix &operator=(Matrix const &) = default; // copy assignment
  ~Matrix() = default;

  static constexpr size_t get_order() { return N; } // number of dimensions

  size_t extent(size_t n) const {
    return desc.extents[n];
  } // # elements in the nth dimension

  template <typename U>
  Matrix(const Matrix_ref<U, N> &); // construct from Matrix_ref

  template <typename U>
  Matrix &operator=(const Matrix_ref<U, N> &); // assign from Matrix_ref

  template <typename... Exts>    // specify the extents
  explicit Matrix(Exts... exts); // init from dims

  Matrix(Matrix_initializer<T, N>);            // initializer from list
  Matrix &operator=(Matrix_initializer<T, N>); // assign from list

  template <typename U>
  Matrix(std::initializer_list<U>) = delete; // don't use {} except for elements

  template <typename U> Matrix &operator=(std::initializer_list<U>) = delete;

  friend std::ostream &operator<<(std::ostream &os, const Matrix &m);

  size_t size() const { return elements.size(); } // total number of elements

  const Matrix_slice<N> &descriptor() const {
    return desc;
  } // the slice defining subscripting

  T *data() { return elements.data(); } // "flat" element access

  const T *data() const { return elements.data(); }

  // template<typename F>
  // Matrix& apply(F f);

  // template<typename M, typename F>
  // Matrix& apply(const M& m, F f);

  // Matrix& operator=(const T& value);
  // Matrix& operator+=(const T& value);
  // Matrix& operator-+(const T & value);
  // Matrix& operator*+(const T & value);
  // Matrix& operator/+(const T & value);

  Matrix_ref<T, N - 1> row(size_t n);

  Matrix_ref<T, N - 1> column(size_t n);

  template <typename... Args>
  Enable_if<Matrix_impl::Requesting_element<Args...>(), T &>
  operator()(Args... args);

  template <typename... Args>
  Enable_if<Matrix_impl::Requesting_element<Args...>(), Matrix_ref<T, N>>
  operator()(const Args &...args);

  std::ostream& print_matrix(std::ostream& out, const Matrix &m);
private:
  Matrix_slice<N> desc;
  std::vector<T> elements;
};

template <size_t N>
template <typename... Dims>
inline Matrix_slice<N>::Matrix_slice(Dims... dims)
    : extents{size_t(dims)...}, strides{N} {
  static_assert(sizeof...(Dims) == N, "");
  size = (dims * ...);
  strides = Matrix_impl::computing_stride<N>(extents);
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

  size = Matrix_impl::computing_size<N>(extents);

  strides = Matrix_impl::computing_stride<N>(extents);
}

template <size_t N>
inline Matrix_slice<N>::Matrix_slice(size_t s,
                                     std::initializer_list<size_t> exts,
                                     std::initializer_list<size_t> strs)
    : extents{exts}, strides{strs}, start{s} {
  static_assert(exts.size() == N, "");
  size = Matrix_impl::computing_stride<N>(extents);
}

template <typename T, size_t N>
template <typename... Exts>
inline Matrix<T, N>::Matrix(Exts... exts)
    : desc{size_t(exts)...}, elements(desc.size) {}

template <typename T, size_t N>
Matrix<T, N>::Matrix(Matrix_initializer<T, N> list) {
  std::array<size_t, N> extents = Matrix_impl::derive_extents<N>(list);
  std::array<size_t, N> strides = Matrix_impl::computing_stride<N>(extents);

  desc.extents = extents;
  desc.strides = strides;
  desc.size = Matrix_impl::computing_size<N>(extents);
  Matrix_impl::insert_flat(list, this->elements);
}

template <typename T, size_t N>
std::ostream &operator<<(std::ostream &os, const Matrix<T, N> &m) {

  os << m.get_order();
  return os;
}
template <typename T, size_t N>
std::ostream& Matrix<T, N>::print_matrix(std::ostream& out, const Matrix<T, N> &m){
  
}

template <typename T, size_t N>
Matrix_ref<T, N - 1> Matrix<T, N>::row(size_t n) {
  assert(n < rows());
  Matrix_slice<N - 1> row;
  Matrix_impl::slice_dim<0>(n, desc, row);
  return {row, data()};
}

template <typename T, size_t N>
Matrix_ref<T, N - 1> Matrix<T, N>::column(size_t n) {
  assert(n < cols());
  Matrix_slice<N - 1> col;
  Matrix_impl::slice_dim<1>(n, desc, col);
  return {col, data()};
}

template <typename T, size_t N>
template <typename... Args>
Enable_if<Matrix_impl::Requesting_element<Args...>(), T &>
Matrix<T, N>::operator()(Args... args) {
  assert(Matrix_impl::check_bounds(desc, args...));
  return *(data() + desc(args...));
}

template <typename T, size_t N>
template <typename... Args>
Enable_if<Matrix_impl::Requesting_element<Args...>(), Matrix_ref<T, N>>
Matrix<T, N>::operator()(const Args &...args) {
  Matrix_slice<N> d;
  d.start = Matrix_impl::do_slice(desc, d, args...);
}

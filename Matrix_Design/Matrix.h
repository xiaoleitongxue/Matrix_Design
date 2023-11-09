#pragma once
#include "Matrix.h"
#include "Matrix_base.h"
#include "Matrix_ref.h"
#include "Matrix_slice.h"
#include "common.h"
#include <ostream>
#include <stdio.h>
#include <vcruntime.h>

template <typename T, size_t N>
using Matrix_initializer = typename Matrix_impl::Matrix_init<T, N>::type;

template <typename T, size_t N> class Matrix;
template <typename T> class Matrix<T, 0>;
template <typename T> class Matrix<T, 1>;

template <typename T> class Matrix<T, 0> {
public:
  static constexpr size_t order = 0;
  using value_type = T;

  Matrix(const T &x) : elem(x) {}
  Matrix &operator=(const T &value) {
    elem = value;
    return *this;
  }

  T &operator()() { return elem; }
  T &operator()() const { return elem; }

  operator T &() { return elem; }
  operator const T &() { return elem; }

  T &row(size_t n) = delete;

private:
  T elem;
};

template <typename T> class Matrix<T, 1> {
public:
  static constexpr size_t order = 1;
  using value_type = T;

  Matrix(const T &x) : elems(x) {}
  Matrix &operator=(const Matrix &value) { return *this; }

  //   T &operator()() { return elem; }
  //   T &operator()() const { return elem; }

  //   operator T &() { return elem; }
  //   operator const T &() { return elem; }

  T &row(size_t i);

private:
  Matrix_slice<1> desc;
  std::vector<T> elems;
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

  size_t size() const { return elems.size(); } // total number of elements

  const Matrix_slice<N> &descriptor() const {
    return desc;
  } // the slice defining subscripting

  T *data() { return elems.data(); } // "flat" element access

  const T *data() const { return elems.data(); }

  // template<typename F>
  // Matrix& apply(F f);

  // template<typename M, typename F>
  // Matrix& apply(const M& m, F f);

  // Matrix& operator=(const T& value);
  // Matrix& operator+=(const T& value);
  // Matrix& operator-+(const T & value);
  // Matrix& operator*+(const T & value);
  // Matrix& operator/+(const T & value);

  Matrix_ref<T, N - 1> operator[](size_t i) { return row(i); }
  Matrix_ref<const T, N - 1> operator[](size_t i) const { return row(i); }

  Matrix_ref<T, N - 1> row(size_t n);
  Matrix_ref<const T, N - 1> row(size_t n) const;

  Matrix_ref<T, N - 1> col(size_t n);
  Matrix_ref<const T, N - 1> col(size_t n) const;

  template <typename... Args>
  Enable_if<Requesting_element<Args...>(), T &> operator()(Args... args);

  template <typename... Args>
  Enable_if<Requesting_element<Args...>(), T &> operator()(Args... args) const;

  template <typename... Args>
  Enable_if<Requesting_slice<Args...>(), Matrix_ref<T, N>>
  operator()(const Args &...args);

  template <typename... Args>
  Enable_if<Requesting_slice<Args...>(), Matrix_ref<T, N>>
  operator()(const Args &...args) const;

  int rows() {
    int rs = 1;
    if (N == 1) {
      return 1;
    }

    for (size_t i = 1; i < N; ++i) {
      rs *= desc.extents[i];
    }
    return rs;
  }

  int cols() { return desc.extents[desc.extents.size() - 1]; }

private:
  Matrix_slice<N> desc;
  std::vector<T> elems;
};

template <typename T, size_t N>
template <typename... Exts>
inline Matrix<T, N>::Matrix(Exts... exts)
    : desc{size_t(exts)...}, elems(desc.size) {}

template <typename T, size_t N>
Matrix<T, N>::Matrix(Matrix_initializer<T, N> list) {
  std::array<size_t, N> extents = Matrix_impl::derive_extents<N>(list);
  std::array<size_t, N> strides = computing_stride<N>(extents);

  desc.extents = extents;
  desc.strides = strides;
  desc.size = computing_size<N>(extents);
  Matrix_impl::insert_flat(list, this->elements);
}

template <typename T, size_t N>
std::ostream &operator<<(std::ostream &os, const Matrix<T, N> &m) {

  os << print_matrix<N>(os, m);
  return os;
}
template <typename T, size_t N, typename... Args>
Enable_if<(N > 1), std::ostream &>
print_matrix(std::ostream &out, const Matrix<T, N> &m, Args... args) {
  out << '[';
  for (size_t i = 0; i < 5; ++i) {
    print_matrix<N - 1>(out, m, args..., i);
  }
  out << ']';
  return out;
}

template <typename T, size_t N, typename... Args>
Enable_if<(N == 1), std::ostream &>
print_matrix(std::ostream &out, const Matrix<T, N> &m, Args... args) {

  out << '[';
  for (size_t i = 0; i < 3; ++i) {
    std::cout << m(args..., i) << ", ";
  }
  out << "]," << std::endl;
  return out;
}

template <typename T, size_t N>
Matrix_ref<T, N - 1> Matrix<T, N>::row(size_t n) {
  assert(n < rows());
  Matrix_slice<N - 1> row;
  slice_dim<0>(n, desc, row);
  return {row, data()};
}

template <typename T> T &Matrix<T, 1>::row(size_t i) { return &elems[i]; }

template <typename T, size_t N>
Matrix_ref<T, N - 1> Matrix<T, N>::col(size_t n) {
  assert(n < cols());
  Matrix_slice<N - 1> col;
  slice_dim<1>(n, desc, col);
  return {col, data()};
}

template <typename T, size_t N>
template <typename... Args>
Enable_if<Requesting_element<Args...>(), T &>
Matrix<T, N>::operator()(Args... args) {
  assert(check_bounds(desc, args...));
  return *(data() + desc(args...));
}

template <typename T, size_t N>
template <typename... Args>
Enable_if<Requesting_slice<Args...>(), Matrix_ref<T, N>>
Matrix<T, N>::operator()(const Args &...args) {
  Matrix_slice<N> d;
  d.start = do_slice(desc, d, args...);
  return {d.data()};
}
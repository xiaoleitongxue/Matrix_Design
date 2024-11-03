#pragma once

#include "common.h"
#include "matrix_base.h"
#include "matrix_ref.h"
#include "matrix_slice.h"
#include <array>
#include <cassert>
#include <initializer_list>
#include <ostream>

template <typename T, std::size_t N>
using Matrix_initializer = typename matrix_impl::Matrix_init<T, N>::type;

template <typename T, std::size_t N> class Matrix;
template <typename T> class Matrix<T, 0>;
template <typename T> class Matrix<T, 1>;

// scalar
template <typename T> class Matrix<T, 0> {
public:
  static constexpr std::size_t order = 0;
  using value_type = T;

  Matrix() = default;               // constructor
  Matrix(const Matrix &) = default; // copy constructor
  auto operator=(const Matrix &)
      -> Matrix & = default;                       // copy assignment operator
  Matrix(Matrix &&) = default;                     // move constructor
  auto operator=(Matrix &&) -> Matrix & = default; // move assignment operator

  explicit Matrix(const T &value) : elem(value) {}

  auto operator=(const T &value) -> Matrix & {
    elem = value;
    return *this;
  }

  auto operator()() -> T & { return elem; }

  auto operator()() const -> T & { return elem; }

  explicit operator T &() { return elem; }
  explicit operator const T &() { return elem; }

  auto row(std::size_t n) -> T & = delete;

private:
  T elem;
};

template <typename T> class Matrix<T, 1> {
public:
  static constexpr std::size_t order = 1;
  using value_type = T;

  Matrix() = default;                                   // default constructor
  Matrix(Matrix const &) = default;                     // copy constructor
  auto operator=(Matrix const &) -> Matrix & = default; // copy assignment
  Matrix(Matrix &&) = default;                          // move constructor
  auto operator=(Matrix &&) -> Matrix & = default;      // move assignment
  ~Matrix() = default;

  // constructor
  explicit Matrix(const T &n);

  explicit Matrix(Matrix_initializer<T, 1> /*list*/); // initializer from list
  auto operator=(Matrix_initializer<T, 1> /*list*/)
      -> Matrix &; // assign from list

  template <typename U>
  explicit Matrix(
      Matrix_ref<U, 1> const & /*m_r*/); // construct from Matrix_ref
  template <typename U>
  auto operator=(Matrix_ref<U, 1> const & /*m_r*/)
      -> Matrix &; // assign from Matrix_ref

  auto operator()(std::size_t index) -> T & { return elems[index]; }

  auto row(std::size_t index) -> T & = delete;

  auto column(std::size_t index) -> T & = delete;

  auto data() -> T * { return elems.data(); } // "flat" element access

  auto data() const -> const T * { return elems.data(); }

  template <typename T1, std::size_t N1>
  friend auto operator<<(std::ostream &ost, const Matrix<T, 1> &matrix)
      -> std::ostream &;
  [[nodiscard]] auto descriptor() const -> const Matrix_slice<1> & {
    return desc;
  } // the slice defining subscripting
private:
  Matrix_slice<1> desc;
  std::vector<T> elems;
};

template <typename T> Matrix<T, 1>::Matrix(const T &n) : desc(1), elems(n) {}

template <typename T>
template <typename U>
Matrix<T, 1>::Matrix(Matrix_ref<U, 1> const &m_r) : desc{m_r.descriptor()} {
  T *first = m_r.pointer() + desc.start;
  matrix_impl::insert_from_m_r<T, 1>(first, desc.extents, desc.strides, elems);
}

template <typename T>
template <typename U>
auto Matrix<T, 1>::operator=(Matrix_ref<U, 1> const &m_r) -> Matrix<T, 1> & {
  desc = m_r.descriptor();
  T *first = m_r.pointer() + desc.start;
  matrix_impl::insert_from_m_r<T, 1>(first, desc.extents, desc.strides, elems);
  return *this;
}

template <typename T, std::size_t N> class Matrix : public Matrix_base<T, N> {
  // special to matrix
public:
  static constexpr std::size_t order = N; // dimensions
  Matrix() = default;

  Matrix(Matrix &&) = default;                          // move constructor
  auto operator=(Matrix &&) -> Matrix & = default;      // move assignment
  Matrix(Matrix const &) = default;                     // copy constructor
  auto operator=(Matrix const &) -> Matrix & = default; // copy assignment
  ~Matrix() = default;

  static constexpr auto get_order() -> std::size_t {
    return N;
  } // number of order

  [[nodiscard]] auto extent(std::size_t n) const -> std::size_t {
    return desc.extents[n];
  } // # elements in the nth dimension

  template <typename U>
  explicit Matrix(
      const Matrix_ref<U, N> & /*m_r*/); // construct from Matrix_ref
  template <typename U>
  auto operator=(const Matrix_ref<U, N> & /*m_r*/)
      -> Matrix &; // assign from Matrix_ref

  explicit Matrix(Matrix_initializer<T, N> /*list*/); // initializer from list
  auto operator=(Matrix_initializer<T, N> /*list*/)
      -> Matrix &; // assign from list

  template <typename... Extents>
  explicit Matrix(Extents... extents); // init from dims
  // disable init Matrix from std::initializer_list<T> or
  // std::initializer_list<std::initializer_list<D>> because Matrix<T, N>,
  // where N > 2, can only be init from 3D std::initializer_list.
  // std::initializer_list<U> match type std::initializer_list<U> and
  // std::initializer_list<std::initializer_list<D>>
  template <typename U> Matrix(std::initializer_list<U>) = delete;

  template <typename U>
  auto operator=(std::initializer_list<U>) -> Matrix & = delete;

  template <typename T1, std::size_t N1>
  friend auto operator<<(std::ostream &ost, const Matrix<T1, N1> &matrix)
      -> std::ostream &;

  [[nodiscard]] auto size() const -> std::size_t {
    return elems.size();
  } // total number of elements

  auto descriptor() const -> const Matrix_slice<N> & {
    return desc;
  } // the slice defining subscripting

  auto data() -> T * { return elems.data(); } // "flat" element access

  auto data() const -> const T * { return elems.data(); }

  auto operator[](std::size_t index) -> Matrix_ref<T, N - 1> {
    return row(index);
  }
  auto operator[](std::size_t index) const -> Matrix_ref<const T, N - 1> {
    return row(index);
  }

  auto row(std::size_t n) -> Matrix_ref<T, N - 1>;

  auto row(std::size_t n) const -> Matrix_ref<const T, N - 1>;

  auto col(std::size_t n) -> Matrix_ref<T, N - 1>;

  auto col(std::size_t n) const -> Matrix_ref<const T, N - 1>;

  template <typename... Args>
  auto operator()(Args... args)
      -> Enable_if<matrix_impl::Requesting_element<Args...>(), T &>;

  template <typename... Args>
  auto operator()(Args... args) const
      -> Enable_if<matrix_impl::Requesting_element<Args...>(), T &>;

  template <typename... Args>
  auto operator()(const Args &...args)
      -> Enable_if<Requesting_slice<Args...>(), Matrix_ref<T, N>>;

  template <typename... Args>
  auto operator()(const Args &...args) const
      -> Enable_if<Requesting_slice<Args...>(), Matrix_ref<T, N>>;

  auto rows() -> int { return desc.extents[0]; }

  auto cols() -> int { return desc.extents[1]; }

private:
  Matrix_slice<N> desc;
  std::vector<T> elems;
};

template <typename T, std::size_t N>
template <typename U>
Matrix<T, N>::Matrix(const Matrix_ref<U, N> &m_r) {
  desc = m_r.descriptor();
  T *first = m_r.pointer() + desc.start;
  matrix_impl::insert_from_m_r<T, N>(first, desc.extents, desc.strides, elems);
  desc.strides = matrix_impl::computing_stride<N>(desc.extents);
  desc.start = 0;
}

template <typename T, std::size_t N>
template <typename U>
auto Matrix<T, N>::operator=(const Matrix_ref<U, N> &m_r) -> Matrix<T, N> & {
  desc = m_r.get_matrix_desc();
  T *first = m_r.get_first_element_ptr() + desc.start;
  matrix_impl::insert_from_m_r<T, N, std::vector<T>>(first, desc.extents,
                                                     desc.strides, this->elems);
  desc.strides = matrix_impl::computing_stride<N>(desc.extents);
  desc.start = 0;
  return *this;
}

template <typename T, std::size_t N>
template <typename... Extents>
Matrix<T, N>::Matrix(Extents... extents)
    : desc{static_cast<std::size_t>(extents)...}, elems(desc.size) {}

template <typename T> Matrix<T, 1>::Matrix(Matrix_initializer<T, 1> list) {
  std::array<std::size_t, 1> extents = matrix_impl::derive_extents<1>(list);
  std::array<std::size_t, 1> strides =
      matrix_impl::computing_stride<1>(extents);
  desc.start = 0;
  desc.extents = extents;
  desc.strides = strides;
  desc.size = matrix_impl::computing_size<1>(extents);
  matrix_impl::insert_flat(list, this->elems);
}

template <typename T>
auto Matrix<T, 1>::operator=(Matrix_initializer<T, 1> list) -> Matrix<T, 1> & {
  std::array<std::size_t, 1> extents = matrix_impl::derive_extents<1>(list);
  std::array<std::size_t, 1> strides =
      matrix_impl::computing_stride<1>(extents);
  desc.start = 0;
  desc.extents = extents;
  desc.strides = strides;
  desc.size = matrix_impl::computing_size<1>(extents);
  matrix_impl::insert_flat(list, this->elems);
  return *this;
}

template <typename T, std::size_t N>
Matrix<T, N>::Matrix(Matrix_initializer<T, N> list) {
  std::array<std::size_t, N> extents = matrix_impl::derive_extents<N>(list);
  std::array<std::size_t, N> strides =
      matrix_impl::computing_stride<N>(extents);
  desc.start = 0;
  desc.extents = extents;
  desc.strides = strides;
  desc.size = matrix_impl::computing_size<N>(extents);
  matrix_impl::insert_flat(list, this->elems);
}

template <typename T, std::size_t N>
inline auto Matrix<T, N>::operator=(Matrix_initializer<T, N> list)
    -> Matrix<T, N> & {
  // TODO: insert return statement here
  std::array<std::size_t, N> extents = matrix_impl::derive_extents<N>(list);
  std::array<std::size_t, N> strides = computing_stride<N>(extents);
  desc.start = 0;
  desc.extents = extents;
  desc.strides = strides;
  desc.size = computing_size<N>(extents);
  matrix_impl::insert_flat(list, this->elems);
  return *this;
}

template <typename T1, std::size_t N1>
auto operator<<(std::ostream &ost, const Matrix<T1, N1> &matrix)
    -> std::ostream & {
  print_matrix<N1>(ost, matrix, 0);
  return ost;
}

template <std::size_t dim, typename T1, std::size_t N1>
auto print_matrix(std::ostream &out, const Matrix<T1, N1> &matrix,
                  std::size_t offset) -> Enable_if<(dim == 1), void> {
  out << "[";
  for (std::size_t i = 0; i < matrix.descriptor().extents[N1 - dim]; ++i) {
    out << *(matrix.data() + offset + i);
    if (i < matrix.descriptor().extents[N1 - dim] - 1) {
      out << ", ";
    }
  }
  out << "]";
}

template <std::size_t dim, typename T1, std::size_t N1>
auto print_matrix(std::ostream &out, const Matrix<T1, N1> &matrix,
                  std::size_t offset) -> Enable_if<(dim == 2), void> {
  out << "[";
  for (std::size_t i = 0; i < matrix.descriptor().extents[N1 - dim]; ++i) {
    if (i > 0) {
      for (std::size_t j = 0; j < N1 - dim + 1; j++) {
        out << " ";
      }
    }
    std::size_t offset_ = offset + i * matrix.descriptor().strides[N1 - dim];
    print_matrix<dim - 1>(out, matrix, offset_);
    if (i < matrix.descriptor().extents[N1 - dim] - 1) {
      out << "," << '\n';
    }
  }
  out << "]";
}

template <std::size_t dim, typename T1, std::size_t N1>
auto print_matrix(std::ostream &out, const Matrix<T1, N1> &matrix,
                  std::size_t offset) -> Enable_if<(dim > 2), void> {
  out << "[";
  for (std::size_t i = 0; i < matrix.descriptor().extents[N1 - dim]; ++i) {
    if (i > 0) {
      for (std::size_t j = 0; j < N1 - dim + 1; j++) {
        out << " ";
      }
    }
    std::size_t offset_ = offset + i * matrix.descriptor().strides[N1 - dim];
    print_matrix<dim - 1>(out, matrix, offset_);
    if (i < matrix.descriptor().extents[N1 - dim] - 1) {
      out << "," << '\n' << '\n';
    }
  }
  out << "]";
}

template <typename T, std::size_t N>
auto Matrix<T, N>::row(std::size_t n) -> Matrix_ref<T, N - 1> {
  assert(n < rows());
  Matrix_slice<N - 1> row;
  slice_dim<0, T, N>(n, desc, row);
  return {row, data()};
}

template <typename T, std::size_t N>
inline auto Matrix<T, N>::row(std::size_t n) const
    -> Matrix_ref<const T, N - 1> {
  assert(n < rows());
  Matrix_slice<N - 1> row;
  slice_dim<0, T, N>(n, desc, row);
  return {row, data()};
}

template <typename T, std::size_t N>
auto Matrix<T, N>::col(std::size_t n) -> Matrix_ref<T, N - 1> {
  assert(n < cols());
  Matrix_slice<N - 1> col;
  slice_dim<1>(n, desc, col);
  return {col, data()};
}

template <typename T, std::size_t N>
inline auto Matrix<T, N>::col(std::size_t n) const
    -> Matrix_ref<const T, N - 1> {
  assert(n < cols());
  Matrix_slice<N - 1> col;
  slice_dim<1>(n, desc, col);
  return {col, data()};
}

template <typename T, std::size_t N>
template <typename... Args>
auto Matrix<T, N>::operator()(Args... args)
    -> Enable_if<matrix_impl::Requesting_element<Args...>(), T &> {
  assert(matrix_impl::check_bounds<N>(desc.extents, args...));
  return *(data() + desc(args...));
}

template <typename T, std::size_t N>
template <typename... Args>
inline auto Matrix<T, N>::operator()(Args... args) const
    -> Enable_if<matrix_impl::Requesting_element<Args...>(), T &> {
  assert(check_bounds(desc, args...));
  return *(data() + desc(args...));
}

template <typename T, std::size_t N>
template <typename... Args>
inline auto Matrix<T, N>::operator()(const Args &...args)
    -> Enable_if<Requesting_slice<Args...>(), Matrix_ref<T, N>> {
  Matrix_slice<N> descriptor;
  descriptor.start = do_slice(desc, descriptor, args...);
  descriptor.size = matrix_impl::computing_size<N>(descriptor.extents);
  return {descriptor, this->data()};
}

template <typename T, std::size_t N>
template <typename... Args>
inline auto Matrix<T, N>::operator()(const Args &...args) const
    -> Enable_if<Requesting_slice<Args...>(), Matrix_ref<T, N>> {
  Matrix_slice<N> descriptor;
  descriptor.start = do_slice(desc, descriptor, args...);
  descriptor.size = matrix_impl::computing_size<N>(descriptor.extents);
  return {descriptor, this->data()};
}

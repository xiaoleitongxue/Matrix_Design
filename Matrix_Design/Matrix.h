#pragma once

#include "Matrix_base.h"
#include "Matrix_ref.h"
#include "Matrix_slice.h"
#include "common.h"
#include <array>
#include <initializer_list>

#include <ostream>

#include <cassert>

template<typename T, size_t N>
using Matrix_initializer = typename Matrix_impl::Matrix_init<T, N>::type;

template<typename T, size_t N>
class Matrix;
template<typename T>
class Matrix<T, 0>;
template<typename T>
class Matrix<T, 1>;

// scalar
template<typename T>
class Matrix<T, 0> {
public:
    static constexpr size_t order = 0;
    using value_type = T;
    Matrix() = default; // constructor

    Matrix(const Matrix &) = default; // copy constructor

    Matrix &operator=(const Matrix &) = default; // copy assignment operator

    Matrix(Matrix &&) = default; // move constructor

    Matrix &operator=(Matrix &&) = default; // move assignment operator

    explicit Matrix(const T &value)
        : elem(value) {
    }

    Matrix &operator=(const T &value) {
        elem = value;
        return *this;
    }

    T &operator()() { return elem; }

    T &operator()() const { return elem; }

    explicit operator T &() { return elem; }
    explicit operator const T &() { return elem; }

    T &row(size_t n) = delete;

private:
    T elem;
};

template<typename T>
class Matrix<T, 1> {
public:
    static constexpr size_t order = 1;
    using value_type = T;

    Matrix() = default; // default constructor
    Matrix(Matrix const &) = default; // copy constructor
    Matrix &operator=(Matrix const &) = default; // copy assignment
    Matrix(Matrix &&) = default; // move constructor
    Matrix &operator=(Matrix &&) = default; // move assignment
    ~Matrix() = default;

    // constructor
    explicit Matrix(const T &);
    explicit Matrix(Matrix_initializer<T, 1>); // initializer from list
    Matrix &operator=(Matrix_initializer<T, 1>); // assign from list

    template<typename U>
    explicit Matrix(Matrix_ref<U, 1> const &); // construct from Matrix_ref
    template<typename U>
    Matrix &operator=(Matrix_ref<U, 1> const &); // assign from Matrix_ref

    T &operator()(size_t i) { return elems[i]; }


    T &row(size_t i) = delete;
    T &column(size_t j) = delete;
    T *data() { return elems.data(); } // "flat" element access

    const T *data() const { return elems.data(); }

    template<typename T1, size_t N1>
    friend std::ostream &operator<<(std::ostream &os, const Matrix<T, 1> &m);

private:
    Matrix_slice<1> desc;
    std::vector<T> elems;
};

template<typename T>
Matrix<T, 1>::Matrix(const T &n)
    : desc(1)
      , elems(n) {
}

template<typename T>
template<typename U>
Matrix<T, 1>::Matrix(Matrix_ref<U, 1> const &m_r)
    : desc{m_r.descriptor()} {
    T *first = m_r.pointer() + desc.start;
    Matrix_impl::insert_from_m_r<T, 1>(first, desc.extents, desc.strides, elems);
}

template<typename T>
template<typename U>
Matrix<T, 1> &Matrix<T, 1>::operator=(Matrix_ref<U, 1> const &m_r) {
    desc = m_r.descriptor();
    T *first = m_r.pointer() + desc.start;
    Matrix_impl::insert_from_m_r<T, 1>(first, desc.extents, desc.strides, elems);
    return *this;
}

template<typename T, size_t N>
class Matrix : public Matrix_base<T, N> {
    // special to matrix
public:
    static constexpr size_t order = N; // dimensions
    Matrix() = default;

    Matrix(Matrix &&) = default; // move constructor
    Matrix &operator=(Matrix &&) = default; // move assignment
    Matrix(Matrix const &) = default; // copy constructor
    Matrix &operator=(Matrix const &) = default; // copy assignment
    ~Matrix() = default;

    static constexpr size_t get_order() { return N; } // number of order

    [[nodiscard]] size_t extent(size_t n) const {
        return desc.extents[n];
    } // # elements in the nth dimension

    template<typename U>
    explicit Matrix(const Matrix_ref<U, N> &); // construct from Matrix_ref
    template<typename U>
    Matrix &operator=(const Matrix_ref<U, N> &); // assign from Matrix_ref

    explicit Matrix(Matrix_initializer<T, N>); // initializer from list
    Matrix &operator=(Matrix_initializer<T, N>); // assign from list

    template<typename... Extents> // specify the extents
    explicit Matrix(Extents... extents); // init from dims

    template<typename U>
    Matrix(std::initializer_list<U>) = delete; // don't use {} except for elements
    template<typename U>
    Matrix &operator=(std::initializer_list<U>) = delete;

    template<typename T1, size_t N1>
    friend std::ostream &operator<<(std::ostream &os, const Matrix<T1, N1> &m);

    [[nodiscard]] size_t size() const { return elems.size(); } // total number of elements

    const Matrix_slice<N> &descriptor() const {
        return desc;
    } // the slice defining subscripting

    T *data() { return elems.data(); } // "flat" element access

    const T *data() const { return elems.data(); }

    Matrix_ref<T, N - 1> operator[](size_t i) { return row(i); }
    Matrix_ref<const T, N - 1> operator[](size_t i) const { return row(i); }

    Matrix_ref<T, N - 1> row(size_t n);

    Matrix_ref<const T, N - 1> row(size_t n) const;

    Matrix_ref<T, N - 1> col(size_t n);

    Matrix_ref<const T, N - 1> col(size_t n) const;

    template<typename... Args>
    Enable_if<Matrix_impl::Requesting_element<Args...>(), T &>
    operator()(Args... args);

    template<typename... Args>
    Enable_if<Matrix_impl::Requesting_element<Args...>(), T &>
    operator()(Args... args) const;

    template<typename... Args>
    Enable_if<Requesting_slice<Args...>(), Matrix_ref<T, N> >
    operator()(const Args &... args);

    template<typename... Args>
    Enable_if<Requesting_slice<Args...>(), Matrix_ref<T, N> >
    operator()(const Args &... args) const;

    int rows() { return desc.extents[0]; }

    int cols() { return desc.extents[1]; }

private:
    Matrix_slice<N> desc;
    std::vector<T> elems;
};

template<typename T, size_t N>
template<typename U>
Matrix<T, N>::Matrix(const Matrix_ref<U, N> &m_r) {
    desc = m_r.descriptor();
    T *first = m_r.pointer() + desc.start;
    Matrix_impl::insert_from_m_r<T, N>(first, desc.extents, desc.strides, elems);
    desc.strides = Matrix_impl::computing_stride<N>(desc.extents);
    desc.start = 0;
}

template<typename T, size_t N>
template<typename U>
Matrix<T, N> &Matrix<T, N>::operator=(const Matrix_ref<U, N> &m_r) {
    desc = m_r.get_matrix_desc();
    T *first = m_r.get_first_element_ptr() + desc.start;
    Matrix_impl::insert_from_m_r<T, N, std::vector<T> >(first, desc.extents,
                                                        desc.strides, this->elems);
    desc.strides = Matrix_impl::computing_stride<N>(desc.extents);
    desc.start = 0;
    return *this;
}

template<typename T, size_t N>
template<typename... Extents>
Matrix<T, N>::Matrix(Extents... extents)
    : desc{static_cast<size_t>(extents)...}
      , elems(desc.size) {
}

template<typename T>
Matrix<T, 1>::Matrix(Matrix_initializer<T, 1> list) {
    std::array<size_t, 1> extents = Matrix_impl::derive_extents<1>(list);
    std::array<size_t, 1> strides = Matrix_impl::computing_stride<1>(extents);
    desc.start = 0;
    desc.extents = extents;
    desc.strides = strides;
    desc.size = Matrix_impl::computing_size<1>(extents);
    Matrix_impl::insert_flat(list, this->elems);
}

template<typename T>
Matrix<T, 1> &Matrix<T, 1>::operator=(Matrix_initializer<T, 1> list) {
    std::array<size_t, 1> extents = Matrix_impl::derive_extents<1>(list);
    std::array<size_t, 1> strides = Matrix_impl::computing_stride<1>(extents);
    desc.start = 0;
    desc.extents = extents;
    desc.strides = strides;
    desc.size = Matrix_impl::computing_size<1>(extents);
    Matrix_impl::insert_flat(list, this->elems);
    return *this;
}

template<typename T, size_t N>
Matrix<T, N>::Matrix(Matrix_initializer<T, N> list) {
    std::array<size_t, N> extents = Matrix_impl::derive_extents<N>(list);
    std::array<size_t, N> strides = Matrix_impl::computing_stride<N>(extents);
    desc.start = 0;
    desc.extents = extents;
    desc.strides = strides;
    desc.size = Matrix_impl::computing_size<N>(extents);
    Matrix_impl::insert_flat(list, this->elems);
}

template<typename T, size_t N>
inline Matrix<T, N> &Matrix<T, N>::operator=(Matrix_initializer<T, N> list) {
    // TODO: insert return statement here
    std::array<size_t, N> extents = Matrix_impl::derive_extents<N>(list);
    std::array<size_t, N> strides = computing_stride<N>(extents);
    desc.start = 0;
    desc.extents = extents;
    desc.strides = strides;
    desc.size = computing_size<N>(extents);
    Matrix_impl::insert_flat(list, this->elems);
    return *this;
}

template<typename T1, size_t N1>
std::ostream &operator<<(std::ostream &os, const Matrix<T1, N1> &m) {
    print_matrix<N1>(os, m, 0);

    return os;
}

template<size_t dim, typename T1, size_t N1>
Enable_if<(dim == 1), void>
print_matrix(std::ostream &out, const Matrix<T1, N1> &m, size_t offset) {
    out << "[";
    for (size_t i = 0; i < m.descriptor().extents[N1 - dim]; ++i) {
        out << *(m.data() + offset + i);
        if (i < m.descriptor().extents[N1 - dim] - 1) {
            out << ", ";
        }
    }
    out << "]";
}

template<size_t dim, typename T1, size_t N1>
Enable_if<(dim == 2), void>
print_matrix(std::ostream &out, const Matrix<T1, N1> &m, size_t offset) {
    out << "[";
    for (size_t i = 0; i < m.descriptor().extents[N1 - dim]; ++i) {
        if (i > 0) {
            for (size_t j = 0; j < N1 - dim + 1; j++) {
                out << " ";
            }
        }
        size_t offset_ = offset + i * m.descriptor().strides[N1 - dim];
        print_matrix<dim - 1>(out, m, offset_);
        if (i < m.descriptor().extents[N1 - dim] - 1) {
            out << "," << std::endl;
        }
    }
    out << "]";
}

template<size_t dim, typename T1, size_t N1>
Enable_if<(dim > 2), void>
print_matrix(std::ostream &out, const Matrix<T1, N1> &m, size_t offset) {
    out << "[";
    for (size_t i = 0; i < m.descriptor().extents[N1 - dim]; ++i) {
        if (i > 0) {
            for (size_t j = 0; j < N1 - dim + 1; j++) {
                out << " ";
            }
        }
        size_t offset_ = offset + i * m.descriptor().strides[N1 - dim];
        print_matrix<dim - 1>(out, m, offset_);
        if (i < m.descriptor().extents[N1 - dim] - 1) {
            out << "," << std::endl
                    << std::endl;
        }
    }
    out << "]";
}

template<typename T, size_t N>
Matrix_ref<T, N - 1> Matrix<T, N>::row(size_t n) {
    assert(n < rows());
    Matrix_slice<N - 1> row;
    slice_dim<0, T, N>(n, desc, row);
    return {row, data()};
}

template<typename T, size_t N>
inline Matrix_ref<const T, N - 1> Matrix<T, N>::row(size_t n) const {
    assert(n < rows());
    Matrix_slice<N - 1> row;
    slice_dim<0, T, N>(n, desc, row);
    return {row, data()};
}

template<typename T, size_t N>
Matrix_ref<T, N - 1> Matrix<T, N>::col(size_t n) {
    assert(n < cols());
    Matrix_slice<N - 1> col;
    slice_dim<1>(n, desc, col);
    return {col, data()};
}

template<typename T, size_t N>
inline Matrix_ref<const T, N - 1> Matrix<T, N>::col(size_t n) const {
    assert(n < cols());
    Matrix_slice<N - 1> col;
    slice_dim<1>(n, desc, col);
    return {col, data()};
}

template<typename T, size_t N>
template<typename... Args>
Enable_if<Matrix_impl::Requesting_element<Args...>(), T &>
Matrix<T, N>::operator()(Args... args) {
    assert(Matrix_impl::check_bounds<N>(desc.extents, args...));
    return *(data() + desc(args...));
}

template<typename T, size_t N>
template<typename... Args>
inline Enable_if<Matrix_impl::Requesting_element<Args...>(), T &>
Matrix<T, N>::operator()(Args... args) const {
    assert(check_bounds(desc, args...));
    return *(data() + desc(args...));
}

template<typename T, size_t N>
template<typename... Args>
inline Enable_if<Requesting_slice<Args...>(), Matrix_ref<T, N> >
Matrix<T, N>::operator()(const Args &... args) {
    Matrix_slice<N> d;
    d.start = do_slice(desc, d, args...);
    d.size = Matrix_impl::computing_size<N>(d.extents);
    return {d, this->data()};
}

template<typename T, size_t N>
template<typename... Args>
inline Enable_if<Requesting_slice<Args...>(), Matrix_ref<T, N> >
Matrix<T, N>::operator()(const Args &... args) const {
    Matrix_slice<N> d;
    d.start = do_slice(desc, d, args...);
    d.size = Matrix_impl::computing_size<N>(d.extents);
    return {d, this->data()};
}

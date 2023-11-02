#pragma once
#include "Matrix_slice.h"
#include "Matrix_base.h"
template <typename T, size_t N> class Matrix_ref;
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
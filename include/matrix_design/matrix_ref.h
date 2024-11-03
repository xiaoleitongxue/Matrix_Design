#pragma once
#include "matrix_base.h"
#include "matrix_slice.h"

template <typename T, std::size_t N> class Matrix_ref;
// A Matrix_ref simply points to the elements of its Matrix
template <typename T, std::size_t N>
class Matrix_ref : public Matrix_base<T, N> {
public:
  Matrix_ref() = default;              // default constructor
  Matrix_ref(Matrix_ref &&) = default; // move constructor
  auto operator=(Matrix_ref &&) -> Matrix_ref & = default; // move assignment
  Matrix_ref(Matrix_ref const &) = default;                // copy constructor
  auto operator=(Matrix_ref const &)
      -> Matrix_ref & = default; // copy assignment
  ~Matrix_ref() = default;

  Matrix_ref(const Matrix_slice<N> &s, T *p) : desc{s}, ptr{p} {}
  auto descriptor() const -> const Matrix_slice<N> & { return desc; }
  auto pointer() const -> T * { return ptr; }

private:
  Matrix_slice<N> desc; // the shape of matrix
  T *ptr;               // the first element of its matrix
};
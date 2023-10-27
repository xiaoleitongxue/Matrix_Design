// Matrix_Design.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <array>
namespace Matrix_impl
{
	template <typename T, size_t N>
	struct Matrix_init
	{
		using type = std::initializer_list<typename Matrix_init<T, N - 1>::type>;
	};

	template <typename T>
	struct Matrix_init<T, 1>
	{
		using type = std::initializer_list<T>;
	};
}

template <typename T, size_t N>
using Matrix_initializer = typename Matrix_impl::Matrix_init<T, N>::type;

// TODO: Reference additional headers your program requires here.
#include <vector>
#include <numeric>
template <size_t N>
struct Matrix_slice
{
	Matrix_slice() = default; // empty matrix
	Matrix_slice(size_t s, std::initializer_list<size_t> exts);
	Matrix_slice(size_t s, std::initializer_list<size_t> exts, std::initializer_list<size_t> strs);
	template <typename... Dims>
	Matrix_slice(Dims... dims);
	template <typename... Dims, typename = std::enable_if<All(std::is_convertible<Dims, size_t>()...)>>
	size_t operator()(Dims... dims) const;
	size_t size;				   // total number of elements
	size_t start;				   // starting offset
	std::array<size_t, N> extents; // number of elemens in each dimension
	std::array<size_t, N> strides; // offsets between elements in each dimension
};

template <typename T, size_t N>
class Matrix_base
{
	// common stuff
public:
	using value_type = T;
	using iterator = typename std::vector<T>::iterator;
	using const_iterator = typename std::vector<T>::const_iterator;
};

template <typename T, size_t N>
class Matrix_ref : public Matrix_base<T, N>
{
	// special to matrix_ref
public:
	Matrix_ref(const Matrix_slice<N> &s, T *p) : desc{s}, ptr{p} {}

private:
	Matrix_slice<N> desc; // the shape of matrix
	T *ptr;				  // the first element in the matrix
};

template <typename T, size_t N>
class Matrix : public Matrix_base<T, N>
{
	// special to matrix
public:
	static constexpr size_t order = N; // dim
	Matrix() = default;
	Matrix(Matrix &&) = default;				 // move constructor
	Matrix &operator=(Matrix &&) = default;		 // move assignment
	Matrix(Matrix const &) = default;			 // copy constructor
	Matrix &operator=(Matrix const &) = default; // copy assignment
	~Matrix() = default;

	// static constexpr size_t order() { return N; } // number of dimensions
	size_t extent(size_t n) const { return desc.extents[n]; } // # elements in the nth dimension

	template <typename U>
	Matrix(const Matrix_ref<U, N> &); // construct from Matrix_ref

	template <typename U>
	Matrix &operator=(const Matrix_ref<U, N> &); // assign from Matrix_ref

	template <typename... Exts>	   // specify the extents
	explicit Matrix(Exts... exts); // init from dims

	Matrix(Matrix_initializer<T, N>);			 // initializer from list
	Matrix &operator=(Matrix_initializer<T, N>); // assign from list

	template <typename U>
	Matrix(std::initializer_list<U>) = delete; // don't use {} except for elements

	template <typename U>
	Matrix &operator=(std::initializer_list<U>) = delete;

	size_t size() const { return elements.size(); } // total number of elements

	const Matrix_slice<N> &descriptor() const { return desc; } // the slice defining subscripting

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

private:
	Matrix_slice<N> desc;
	std::vector<T> elements;
};

template <size_t N>
template <typename... Dims>
inline Matrix_slice<N>::Matrix_slice(Dims... dims) : strides{N}
{
	static_assert(sizeof...(Dims) == N, "");
	extents = {size_t(dims)...};
	size = (dims * ... * 1);
	size_t args[N]{size_t(dims)...}; // copy arguments into an array
	size_t temp[N + 2];
	std::fill(temp, temp + N + 2, size_t(1));
	for (int i = 0; i < N; ++i)
	{
		temp[i] = args[i];
	}
	for (int i = N + 2 - 3; i >= 0; --i)
	{
		strides[i] = 1;
		for (int j = N + 2 - 1; j > i; --j)
		{
			strides[i] *= temp[j];
		}
	}
	start = size_t(0);
}

template <size_t N>
template <typename... Dims, typename>
inline size_t Matrix_slice<N>::operator()(Dims... dims) const
{
	static_assert(sizeof...(Dims) == N, "");
	size_t args[N]{size_t(dims)...}; // copy arguments into an array
	return std::inner_product(args, args + N, strides.begin(), size_t(0));
}

template <size_t N>
inline Matrix_slice<N>::Matrix_slice(size_t s, std::initializer_list<size_t> exts) : extents{exts},start{s}
{
	static_assert(exts.size() == N, "");
	// init size
	int product = 1; // 初始化累积乘积为1

	for (int num : exts)
	{
		product *= num;
	}
	size = product;

	// init extents
	// for (size_t i = 0; i < exts.size(); ++i)
	// {
	// 	extents[i] = exts[i];
	// }

	// init strides
	size_t temp[N + 2]{1};
	for (int i = 0; i < N; ++i)
	{
		temp[i] = extents[i];
	}
	for (int i = N + 2 - 2; i >= 0; --i)
	{
		temp[i] = temp[i] * temp[i + 1];
	}

	for (int i = 0; i < N; ++i)
	{
		strides[i] = temp[i];
	}
}

template <size_t N>
inline Matrix_slice<N>::Matrix_slice(size_t s, std::initializer_list<size_t> exts, std::initializer_list<size_t> strs) : extents{exts},strides{strs},start{s}
{
	static_assert(exts.size() == N, "");
	int product = 1; // 初始化累积乘积为1

	for (int num : exts)
	{
		product *= num;
	}
	size = product;
}

template <typename T, size_t N>
template <typename... Exts>
inline Matrix<T, N>::Matrix(Exts... exts) : desc{size_t(exts)...}, elements(desc.size)
{
}

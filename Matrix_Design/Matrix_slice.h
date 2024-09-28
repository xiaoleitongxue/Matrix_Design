#pragma once

#include "common.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <initializer_list>

#include <numeric>
#include <type_traits>

template <size_t N>
struct Matrix_slice;

struct Slice {
    size_t i;
    size_t j {};
    Slice(const size_t i_, const size_t j_)
        : i(i_)
        , j(j_)
    {
    }
    explicit Slice(const size_t i_)
        : i(i_)
    {
    }
};

template <size_t N>
struct Matrix_slice {
    Matrix_slice() = default; // empty matrix
    Matrix_slice(const Matrix_slice&) = default;
    Matrix_slice& operator=(const Matrix_slice&) = default;
    Matrix_slice(Matrix_slice&&) = default;
    Matrix_slice& operator=(Matrix_slice&&) = default;

    Matrix_slice(size_t start, std::initializer_list<size_t> extents); // extents
    Matrix_slice(size_t start, std::initializer_list<size_t> extents,
        std::initializer_list<size_t> strides);

    template <typename... Dims>
    explicit Matrix_slice(Dims... dims); // N extents

    template <
        typename... Dims,
        typename = std::enable_if<
            Matrix_impl::All(std::is_convertible<Dims, size_t>()...)>> // calculate index from                                                   // a set of subscripts
    size_t operator()(Dims... dims) const;
    size_t size {}; // total number of elements
    size_t start {}; // starting offset
    std::array<size_t, N> extents; // number of elemens in each dimension
    std::array<size_t, N> strides; // offsets between elements in each dimension
};

template <size_t N>
template <typename... Dims>
inline Matrix_slice<N>::Matrix_slice(Dims... dims)
    : extents { static_cast<size_t>(dims)... }
    , strides { N }
{
    static_assert(sizeof...(Dims) == N, "Dims must be N");
    size = (dims * ...);
    strides = Matrix_impl::computing_stride<N>(extents);
    start = static_cast<size_t>(0);
}

template <size_t N>
template <typename... Dims, typename>
inline size_t Matrix_slice<N>::operator()(Dims... dims) const
{
    static_assert(sizeof...(Dims) == N, "Number of params must be N");
    size_t args[N] { static_cast<size_t>(dims)... }; // copy arguments into an array
    return std::inner_product(args, args + N, strides.begin(), static_cast<size_t>(0));
}

template <size_t N>
inline Matrix_slice<N>::Matrix_slice(const size_t start,
    std::initializer_list<size_t> extents)
    : start { start }
    , extents { extents }
{
    static_assert(extents.size() == N, "Dims must be N");

    size = Matrix_impl::computing_size<N>(extents);

    strides = Matrix_impl::computing_stride<N>(extents);
}

template <size_t N>
inline Matrix_slice<N>::Matrix_slice(const size_t start,
    std::initializer_list<size_t> extents,
    std::initializer_list<size_t> strides)
    : start { start }
    , extents { extents }
    , strides { strides }
{
    static_assert(extents.size() == N, "Extents size must be N");
    static_assert(strides.size() == N, "Extents size must be N");
}

template <typename... Args>
constexpr bool Requesting_slice()
{
    return Matrix_impl::All((Convertible<Args, size_t>() || Same<Args, Slice>())...) && Matrix_impl::Some(Same<Args, Slice>()...);
}

template <size_t M, size_t N, typename T>
size_t do_slice_dim(const Matrix_slice<N>& os, Matrix_slice<N>& ns,
    const T& s)
{
    size_t i = 0;
    size_t j = 0;
    if constexpr (std::is_same<T, size_t>()) {
        i = s;
        j = N;
    } else if constexpr (std::is_same<T, Slice>()) {
        const auto s_ = static_cast<Slice>(s);
        i = s_.i;
        j = s_.j;
    }

    size_t current_dim = os.extents.size() - M;

    size_t extent = j - i;
    size_t stride = os.strides[current_dim];

    ns.extents[current_dim] = extent;
    ns.strides[current_dim] = stride;

    size_t offset = i * os.strides[current_dim];
    return offset;
}

template <size_t N, typename T, typename... Args>
size_t do_slice(const Matrix_slice<N>& os, Matrix_slice<N>& ns, const T& s,
    const Args&... args)
{
    // do slice with slice s
    size_t m = do_slice_dim<sizeof...(Args) + 1>(os, ns, s);
    size_t n = do_slice(os, ns, args...);
    // m+n is offset
    return m + n;
}
// when args is empty
template <size_t N>
size_t do_slice(const Matrix_slice<N>& os, Matrix_slice<N>& ns)
{
    return 0;
}

// slice dim 0;
template <size_t dim, typename T, size_t N>
Enable_if<(dim == 0), void> slice_dim(size_t n, Matrix_slice<N>& desc,
    Matrix_slice<N - 1>& row)
{
    row.start = n * desc.strides[0];
    std::copy(desc.extents.begin() + 1, desc.extents.end(), row.extents.begin());
    std::copy(desc.strides.begin() + 1, desc.strides.end(), row.strides.begin());
    row.size = Matrix_impl::computing_size<N - 1>(row.extents);
}

// slice dim 1
template <size_t dim, typename T, size_t N>
Enable_if<(dim == 1), void> slice_dim(size_t n, Matrix_slice<N>& desc,
    Matrix_slice<N - 1>& col)
{
    col.start = n * desc.strides[1];
    size_t j = 0;
    for (size_t i = 0; i < N; ++i) {
        if (i == 1) {
            continue;
        }
        col.extents[j++] = desc.extents[i];
        col.strides[j++] = desc.strides[i];
    }
    col.size = Matrix_impl::computing_size<N - 1>(col.extents);
}
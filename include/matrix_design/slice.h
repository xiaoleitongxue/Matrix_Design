#pragma once

#include <cstddef>
struct Slice {
	Slice() :start(-1), length(-1), stride(1) { }
	explicit Slice(size_t s) :start(s), length(-1), stride(1) { }
	Slice(size_t s, size_t l, size_t n = 1) :start(s), length(l), stride(n) { }
	size_t operator()(size_t i) const { return start + i*stride; }
	static Slice all;
	size_t start; // first index
	size_t length; // number of indices included (can be used for range checking)
	size_t stride; // distance between elements in sequence
};
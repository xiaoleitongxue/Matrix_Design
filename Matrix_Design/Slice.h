#pragma once

struct slice {
	slice() :start(−1), length(−1), stride(1) { }
	explicit slice(size_t s) :start(s), length(−1), stride(1) { }
	slice(size_t s, size_t l, size_t n = 1) :start(s), length(l), stride(n) { }
	size_t operator()(size_t i) const { return start + i∗stride; }
	static slice all;
	size_t star t; // first index
	size_t length; // number of indices included (can be used for range checking)
	size_t stride; // distance between elements in sequence
};
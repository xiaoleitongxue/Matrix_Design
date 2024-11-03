#pragma once

#include <cstddef>
struct Slice {
  Slice() : start(-1), length(-1), stride(1) {}
  explicit Slice(std::size_t s) : start(s), length(-1), stride(1) {}
  Slice(std::size_t s, std::size_t l, std::size_t n = 1)
      : start(s), length(l), stride(n) {}
  auto operator()(std::size_t i) const -> std::size_t {
    return start + i * stride;
  }
  static Slice all;
  std::size_t start; // first index
  std::size_t
      length; // number of indices included (can be used for range checking)
  std::size_t stride; // distance between elements in sequence
};
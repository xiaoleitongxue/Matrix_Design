#pragma once

#include <vector>
template <typename T, std::size_t N> class Matrix_base;
template <typename T, std::size_t N> class Matrix_base {
  // common stuff
public:
  using value_type = T;
  using iterator = typename std::vector<T>::iterator;
  using const_iterator = typename std::vector<T>::const_iterator;
};
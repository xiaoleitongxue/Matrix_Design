#include <climits>
#include <concepts>
#include <cstdint>
#include <gtest/gtest.h>
#include <iostream>
#include <type_traits>
#include <utility>

#include "../Matrix_Design.h"
#include <algorithm>
#include <vector>

template <typename T, size_t N>
bool CompareArrays(const std::array<T, N> arr1, const std::array<T, N> arr2,
                   int size) {
  for (int i = 0; i < size; ++i) {
    if (arr1[i] != arr2[i]) {
      return false;
    }
  }
  return true;
}

int sum(int const &value1, int const &value2) { return value1 + value2; }

TEST(Matrix_design_test, Simple_test) { sum(1, 2); }

// TEST(Matrix_design_test, Matrix_0){
//   Matrix<int, 0> m0(0);
// }


// TEST(Matrix_design_test, Matrix_1) {
//   Matrix<int, 1> m1(3);
//   EXPECT_EQ(m1.descriptor().size, 3);
//   EXPECT_EQ(m1.descriptor().start, 0);
//   std::array<size_t, 1> expected_extents{3};
//   std::array<size_t, 1> expected_strides{1};
//   EXPECT_TRUE(CompareArrays(m1.descriptor().extents, expected_extents, 1));
//   EXPECT_TRUE(CompareArrays(m1.descriptor().strides, expected_strides, 1));
// }

// TEST(Matrix_design_test, Matrix_2) {
//   Matrix<int, 2> m2(3, 4);
//   EXPECT_EQ(m2.descriptor().size, 12);
//   EXPECT_EQ(m2.descriptor().start, 0);
//   std::array<size_t, 2> expected_extents{3, 4};
//   std::array<size_t, 2> expected_strides{4, 1};
//   EXPECT_TRUE(CompareArrays(m2.descriptor().extents, expected_extents, 2));
//   EXPECT_TRUE(CompareArrays(m2.descriptor().strides, expected_strides, 2));
// }

TEST(Matrix_design_test, Matrix_3) {

  Matrix<int, 3> m3(3, 4, 2);

  EXPECT_EQ(m3.descriptor().size, 24);
  EXPECT_EQ(m3.descriptor().start, 0);
  std::array<size_t, 3> expected_extents{3, 4, 2};
  std::array<size_t, 3> expected_strides{8, 2, 1};
  EXPECT_TRUE(CompareArrays(m3.descriptor().extents, expected_extents, 3));
  EXPECT_TRUE(CompareArrays(m3.descriptor().strides, expected_strides, 3));
  std::cout << m3 << std::endl;
}

// TEST(Matrix_design_test, Initializer_list_init_test) {
//   //Matrix<int, 1> m1{1, 2, 3};
//   Matrix<int, 2> m2({{1,2,3}, {4,5,6},{7,8,9}});
// }
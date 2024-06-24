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

// int sum(int const &value1, int const &value2) { return value1 + value2; }

// TEST(Matrix_design_test, Simple_test) { sum(1, 2); }

TEST(Matrix_design_test, Matrix_0){
  Matrix<int, 0> m0(100);
  std::cout << m0() << std::endl;
}

 TEST(Matrix_design_test, Matrix_1) {
   Matrix<int, 1> m1(3);
   EXPECT_EQ(m1.order, 1);
   std::cout << m1 << std::endl;
 }

TEST(Matrix_design_test, Matrix_2) {
  Matrix<int, 2> m2(3, 4);
  EXPECT_EQ(m2.descriptor().size, 12);
  EXPECT_EQ(m2.descriptor().start, 0);
  std::array<size_t, 2> expected_extents{3, 4};
  std::array<size_t, 2> expected_strides{4, 1};
  EXPECT_TRUE(CompareArrays(m2.descriptor().extents, expected_extents, 2));
  EXPECT_TRUE(CompareArrays(m2.descriptor().strides, expected_strides, 2));
  std::cout << m2 << std::endl;
}

TEST(Matrix_design_test, Matrix_3) {

  Matrix<double, 3> m3{{{-0.6942, -0.0651, 0.2575},
                        {-0.4983, -0.9691, 1.1729},
                        {-1.2666, 2.3170, 0.0084},
                        {0.7394, 0.1116, -1.3078},
                        {0.2958, -0.1207, -0.2791}},

                       {{-0.5557, 0.0722, -0.4678},
                        {0.3727, -0.7651, -0.9666},
                        {0.6481, 0.9629, 0.3112},
                        {-0.4197, -0.6255, -0.7439},
                        {0.5926, -0.0331, 2.6677}},

                       {{0.5030, -1.0938, -1.5669},
                        {-0.4148, -1.1019, 1.3724},
                        {-0.0708, -0.4031, 1.5732},
                        {-2.1393, 0.8765, -0.4301},
                        {1.0564, 0.0860, 0.9343}},

                       {{0.1494, -2.3578, -1.5592},
                        {-0.1304, 1.9008, 1.5354},
                        {-0.8664, -0.3545, 1.5108},
                        {1.6060, -0.0159, -1.4242},
                        {-0.1424, 0.4836, 0.3930}}};

  EXPECT_EQ(m3.descriptor().size, 60);
  EXPECT_EQ(m3.descriptor().start, 0);
  std::array<size_t, 3> expected_extents{4, 5, 3};
  std::array<size_t, 3> expected_strides{15, 3, 1};
  EXPECT_TRUE(CompareArrays(m3.descriptor().extents, expected_extents, 3));
  EXPECT_TRUE(CompareArrays(m3.descriptor().strides, expected_strides, 3));
  std::cout << m3 << std::endl;
  std::cout << m3.rows() << std::endl;
  std::cout << m3.cols() << std::endl;
}

// TEST(Matrix_design_test, Initializer_list_init_test) {
//   //Matrix<int, 1> m1{1, 2, 3};
//   Matrix<int, 2> m2({{1,2,3}, {4,5,6},{7,8,9}});
// }
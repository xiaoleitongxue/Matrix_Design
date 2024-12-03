# CMakeList.txt : CMake project for Matrix_Design, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.29)
project(Matrix_Design_Test VERSION 0.1 LANGUAGES CXX)


## TODO: Add tests and install targets if needed.
find_package(GTest CONFIG REQUIRED)


add_executable(Matrix_Design_Test Matrix_Design_Test.cpp)
target_link_libraries(Matrix_Design_Test PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_include_directories(Matrix_Design_Test PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_features(Matrix_Design_Test PUBLIC cxx_std_20)
include(GoogleTest)
gtest_discover_tests(Matrix_Design_Test)

# CMakeList.txt : CMake project for Matrix_Design, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.29)
project(Matrix_Design_App VERSION 0.1 LANGUAGES CXX)
# Add source to this project's executable.
add_executable(Matrix_Design_App Matrix_Design_App.cpp)
target_include_directories(Matrix_Design_App PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_features(Matrix_Design_App PUBLIC cxx_std_20)



﻿# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.20)


project ("Matrix_Design")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)
# Include sub-projects.
add_subdirectory ("Matrix_Design")



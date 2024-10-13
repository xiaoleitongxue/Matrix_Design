# CMakeList.txt : CMake project for Matrix_Design, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.30)
project(Matrix_Design_Test VERSION 0.1 LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)


# # For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
## TODO: Add tests and install targets if needed.
add_executable(Matrix_Design_Test Matrix_Design_Test.cpp)
target_link_libraries(Matrix_Design_Test PUBLIC GTest::gtest GTest::gtest_main)
target_include_directories(Matrix_Design_Test PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_features(Matrix_Design_Test PUBLIC cxx_std_20)
include(GoogleTest)
gtest_discover_tests(Matrix_Design_Test)

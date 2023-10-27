# CMakeList.txt : CMake project for Matrix_Design, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (Matrix_Design "Matrix_Design.cpp" "Matrix_Design.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Matrix_Design PROPERTY CXX_STANDARD 20)
endif()
find_package(GTest 1.14.0 REQUIRED PATHS "C:/Users/lilei/build/googletest-1.14.0/install/")
message(STATUS "GTest version: ${GTest_VERSION}")
# TODO: Add tests and install targets if needed.
add_executable(Matrix_Design_Test "Test/Matrix_Design_Test.cpp" "Matrix_Design.h")

target_link_libraries(Matrix_Design_Test PUBLIC GTest::gtest GTest::gtest_main)
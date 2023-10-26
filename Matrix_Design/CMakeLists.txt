# CMakeList.txt : CMake project for Matrix_Design, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (Matrix_Design "Matrix_Design.cpp" "Matrix_Design.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Matrix_Design PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

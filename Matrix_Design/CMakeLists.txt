# CMakeList.txt : CMake project for Matrix_Design, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (Matrix_Design "Matrix_Design.cpp" "Matrix_Design.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Matrix_Design PROPERTY CXX_STANDARD 20)
endif()
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# # For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
# TODO: Add tests and install targets if needed.
add_executable(Matrix_Design_Test "Test/Matrix_Design_Test.cpp" "Matrix_Design.h")

target_link_libraries(Matrix_Design_Test PUBLIC GTest::gtest GTest::gtest_main -lpthread)

include(GoogleTest)
gtest_discover_tests(Matrix_Design_Test)
cmake_minimum_required(VERSION 2.8)
project(STLEXT_AND_TESTS)

enable_testing()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

include_directories(.)
include_directories(contrib)

add_subdirectory(contrib/gtest)
add_subdirectory(stlext)
add_subdirectory(test)

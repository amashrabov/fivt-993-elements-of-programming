cmake_minimum_required(VERSION 2.8)
project(STLEXT_AND_TESTS)

enable_testing()

include_directories(.)
include_directories(contrib)

add_subdirectory(contrib/gtest)

add_subdirectory(stlext)
add_subdirectory(test)

cmake_minimum_required(VERSION 2.8)
project(STLEXT)

enable_testing()

include_directories(.)
include_directories(contrib)

add_subdirectory(contrib/gtest)

add_subdirectory(stlext)
add_subdirectory(test)

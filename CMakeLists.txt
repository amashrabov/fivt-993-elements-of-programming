cmake_minimum_required(VERSION 2.8)
project(STLEXT)

enable_testing()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_subdirectory(stlext)
add_subdirectory(test)
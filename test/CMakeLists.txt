cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_SOURCE_DIR}/stlext)

set (TESTS
  avl_set_test.cpp
  avl_set_multithreaded_test.cpp
)

add_executable (unittests
  ${TESTS}
)

target_link_libraries (
  unittests
  gtest
  gtest_main
  pthread
)

add_test(
  NAME all_unittest
  COMMAND unittests
)

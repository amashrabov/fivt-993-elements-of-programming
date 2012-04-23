cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_SOURCE_DIR})

set (TESTS
  adviced_search_test.cpp
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

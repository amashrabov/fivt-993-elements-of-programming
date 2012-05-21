cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_SOURCE_DIR}/stlext)
include_directories(${CMAKE_SOURCE_DIR}/pds)


set (TESTS
  fenwick_tree_test.cpp
  static_rmq_test.cpp
  static_rsq_test.cpp
  safe_distance_test.cpp
  persistent_heap_test.cpp
  persistent_heap_multithreaded_test.cpp
  adviced_search_test.cpp
  stack_tests.cpp
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

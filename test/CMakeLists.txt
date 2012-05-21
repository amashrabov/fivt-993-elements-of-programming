cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/stlext)
include_directories(${CMAKE_SOURCE_DIR}/pds)

set (TESTS
  stack_tests.cpp
  fenwick_tree_test.cpp
  static_rmq_test.cpp
  static_rsq_test.cpp
  static_rsq_test.cc
  vector_test.cpp
  safe_distance_test.cpp
  simple_seg_tree.h  
  seg_tree_test.cpp
  seg_tree_multithreaded_test.cpp
  left_heap_test.cpp
  left_heap_multithreaded_test.cpp
  persistent_heap_test.cpp
  persistent_heap_multithreaded_test.cpp
  adviced_search_test.cpp
  persistent_trie_test.cpp
  persistent_trie_multithreaded_test.cpp
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

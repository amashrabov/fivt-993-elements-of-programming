cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/stlext)
include_directories(${CMAKE_SOURCE_DIR}/pds)

set (TESTS
  adviced_search_test.cpp
  avl_set_test.cpp
  avl_set_multithreaded_test.cpp
  fenwick_tree_test.cpp
  left_heap_test.cpp
  left_heap_multithreaded_test.cpp
  persistent_heap_test.cpp
  persistent_heap_multithreaded_test.cpp
  persistent_rope_test.cpp
  persistent_treap_test.cpp
  persistent_trie_test.cpp
  persistent_trie_multithreaded_test.cpp
  safe_distance_test.cpp
  segment_tree_test.cpp
  segment_tree_multithreaded_test.cpp
  stack_tests.cpp
  static_rmq_test.cpp
  static_rsq_test.cpp
  vector_test.cpp
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

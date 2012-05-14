cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_SOURCE_DIR}/stlext)
include_directories(${CMAKE_SOURCE_DIR}/pds)

set (TESTS
  #integer_part_of_log2_test.cc
  #it is dangerous test, on WIN it doesn't work

  fenwick_tree_test.cpp
  static_rmq_test.cpp
  static_rsq_test.cpp
  safe_distance_test.cpp
  left_heap_test.cpp
  left_heap_multithreaded_test.cpp
  persistent_heap_test.cpp
  persistent_heap_multithreaded_test.cpp
  adviced_search_test.cpp
)

add_executable (unittests
  ${TESTS}
)

target_link_libraries (
  unittests
  gtest
  gtest_main
)

if(NOT WIN32)
  target_link_libraries(unittests
    pthread
  )
endif(NOT WIN32)

add_test(
  NAME all_unittest
  COMMAND unittests
)

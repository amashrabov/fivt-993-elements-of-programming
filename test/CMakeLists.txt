cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_SOURCE_DIR}/stlext)
include_directories(${CMAKE_SOURCE_DIR}/pds)


set (TESTS

  #integer_part_of_log2_test.cc
  #it is dangerous test, on WIN it doesn't work
  
  static_rmq_test.cc
  static_rsq_test.cc
  safe_distance_test.cc
  persistent_heap_test.cc
  persistent_heap_multithreaded_test.cc
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

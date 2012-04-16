#include <gtest/gtest.h>
#include <string>

#include "stlext/avl_set.hpp"

using namespace stlext;

TEST(set, use_case_string) {
  std::string ss("Some string");
  const char* hw = "Hello, World!";
  char* wtf = "What a Terrible Failure!"; 

  AvlSet<std::string> avl;
  ASSERT_EQ(0u, avl.size());

  // Inserting in diferent ways
  // checking insert, size and contains function
  avl.insert(ss);
  ASSERT_EQ(1u, avl.size());
  ASSERT_EQ(true, avl.contains(ss));
  ASSERT_EQ(false, avl.contains(std::string("sOME STRING")));

  avl.insert(hw);
  ASSERT_EQ(2u, avl.size());
  ASSERT_EQ(true, avl.contains(hw));
  ASSERT_EQ(false, avl.contains(std::string("HEllO, wOrld?")));

  avl.insert(wtf);
  ASSERT_EQ(3u, avl.size());
  ASSERT_EQ(true, avl.contains(wtf));
  ASSERT_EQ(false, avl.contains("What The F**ck!"));


  // Checking erase function
  avl.remove("Hello, World!");
  ASSERT_EQ(2u, avl.size());
  ASSERT_EQ(false, avl.contains(std::string("Hello, World!")));

  avl.remove("Some string");
  EXPECT_EQ(1u, avl.size());
  EXPECT_EQ(false, avl.contains(std::string("Some string"))); 

  avl.remove("What a Terrible Failure!");
  ASSERT_EQ(0u, avl.size());
  ASSERT_EQ(false, avl.contains(std::string("What a Terrible Failure!")));

  avl.remove("No such string");
  ASSERT_EQ(0u, avl.size());
}

TEST(set, increasing_insertions) {
  AvlSet<int> avl;
  
  // There should be assertion in invariant check
  // if something goes wrong
  for (int i = -1000; i <= 0; ++i) {
    ASSERT_EQ(false, avl.contains(i));
    avl.insert(i);
    ASSERT_EQ(true, avl.contains(i));
  }
}

TEST(map, decreasing_insertions) {
  AvlSet<int> avl;

  // There should be assertion in invariant check
  // if something goes wrong
  for (int i = 1000; i >= 0; --i) {
    ASSERT_EQ(false, avl.contains(i));
    avl.insert(i);
    ASSERT_EQ(true, avl.contains(i));
  }
}

TEST(map, random_insertions) {
  AvlSet<int> avl;
  std::srand(123);  

  // There should be assertion in invariant check
  // if something goes wrong
  for (int i = 0; i < 100000; ++i) {
    int r = std::rand() % 100000;
    avl.remove(r);    
    ASSERT_EQ(false, avl.contains(r));
    avl.insert(r);
    ASSERT_EQ(true, avl.contains(r));
  }
}


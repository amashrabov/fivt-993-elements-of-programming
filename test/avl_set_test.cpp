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

TEST(set, decreasing_insertions) {
  AvlSet<int> avl;

  // There should be assertion in invariant check
  // if something goes wrong
  for (int i = 1000; i >= 0; --i) {
    ASSERT_EQ(false, avl.contains(i));
    avl.insert(i);
    ASSERT_EQ(true, avl.contains(i));
  }
}

TEST(set, random_insertions) {
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

TEST(set, persistent_insertions) {
  // 1 = {}
  AvlSet<double> avl_v1;
  
  // 1 = 2 = {}
  AvlSet<double> avl_v2 = avl_v1;
  ASSERT_EQ(0u, avl_v2.size());

  // 1 = {3.1415926} 2 = {}
  avl_v1.insert(3.1415926);
  ASSERT_EQ(1u, avl_v1.size());
  ASSERT_EQ(0u, avl_v2.size());

  // 1 = {3.1415926} 2 = 3 = {}
  AvlSet<double>avl_v3 = avl_v2;
  ASSERT_EQ(1u, avl_v1.size());
  ASSERT_EQ(0u, avl_v2.size());
  ASSERT_EQ(0u, avl_v3.size());

  // 1 = {3.1415926} 2 = {} 3 = {2.7182818}
  avl_v3.insert(2.7182818);
  ASSERT_EQ(1u, avl_v1.size());
  ASSERT_EQ(0u, avl_v2.size());
  ASSERT_EQ(1u, avl_v3.size());

  // 1 = {3.1415926, 2.7182818} 2 = {} 3 = {2.7182818}
  avl_v1.insert(2.7182818);
  ASSERT_EQ(2u, avl_v1.size());
  ASSERT_EQ(0u, avl_v2.size());
  ASSERT_EQ(1u, avl_v3.size());

  ASSERT_EQ(true, avl_v1.contains(3.1415926));
  ASSERT_EQ(false, avl_v2.contains(3.1415926));
  ASSERT_EQ(false, avl_v3.contains(3.1415926));

  ASSERT_EQ(true, avl_v1.contains(2.7182818));
  ASSERT_EQ(false, avl_v2.contains(2.7182818));
  ASSERT_EQ(true, avl_v3.contains(2.7182818));

  // 1 = {3.1415926, 2.7182818} 2 = {3.1415926, 2.7182818, 1.41421356} 3 = {2.7182818}
  avl_v2 = avl_v1;
  avl_v2.insert(1.41421356);
  ASSERT_EQ(2u, avl_v1.size());
  ASSERT_EQ(3u, avl_v2.size());
  ASSERT_EQ(1u, avl_v3.size());

  ASSERT_EQ(true, avl_v1.contains(3.1415926));
  ASSERT_EQ(true, avl_v2.contains(3.1415926));
  ASSERT_EQ(false, avl_v3.contains(3.1415926));

  ASSERT_EQ(true, avl_v1.contains(2.7182818));
  ASSERT_EQ(true, avl_v2.contains(2.7182818));
  ASSERT_EQ(true, avl_v3.contains(2.7182818));

  ASSERT_EQ(false, avl_v1.contains(1.41421356));
  ASSERT_EQ(true, avl_v2.contains(1.41421356));
  ASSERT_EQ(false, avl_v3.contains(1.41421356));
}


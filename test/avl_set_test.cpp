#include <gtest/gtest.h>
#include <string>
#include <set>

#include "pds/avl_set.h"

using namespace pds;

TEST(set, use_case_string) {
  std::string ss("Some string");
  const char* hw = "Hello, World!";
  char* wtf = const_cast<char*>("What a Terrible Failure!"); 

  avl_set<std::string> avl;
  ASSERT_EQ(0u, avl.size());

  // Inserting in diferent ways
  // checking insert, size and contains function
  avl.insert(ss);
  ASSERT_EQ(1u, avl.size());
  ASSERT_TRUE(avl.contains(ss));
  ASSERT_FALSE(avl.contains(std::string("sOME STRING")));

  avl.insert(hw);
  ASSERT_EQ(2u, avl.size());
  ASSERT_TRUE(avl.contains(hw));
  ASSERT_FALSE(avl.contains(std::string("HEllO, wOrld?")));

  avl.insert(wtf);
  ASSERT_EQ(3u, avl.size());
  ASSERT_TRUE(avl.contains(wtf));
  ASSERT_FALSE(avl.contains("What The F**ck!"));


  // Checking erase function
  avl.erase("Hello, World!");
  ASSERT_EQ(2u, avl.size());
  ASSERT_FALSE(avl.contains(std::string("Hello, World!")));

  avl.erase("Some string");
  ASSERT_EQ(1u, avl.size());
  ASSERT_FALSE(avl.contains(std::string("Some string"))); 

  avl.erase("What a Terrible Failure!");
  ASSERT_EQ(0u, avl.size());
  ASSERT_FALSE(avl.contains(std::string("What a Terrible Failure!")));

  avl.erase("No such string");
  ASSERT_EQ(0u, avl.size());
}

TEST(set, increasing_insertions) {
  avl_set<int> avl;
  
  // There should be assertion in invariant check
  // if something goes wrong
  for (int i = -1000; i <= 0; ++i) {
    ASSERT_FALSE(avl.contains(i));
    avl.insert(i);
    ASSERT_TRUE(avl.contains(i));
  }
}

TEST(set, decreasing_insertions) {
  avl_set<int> avl;

  // There should be assertion in invariant check
  // if something goes wrong
  for (int i = 1000; i >= 0; --i) {
    ASSERT_FALSE(avl.contains(i));
    avl.insert(i);
    ASSERT_TRUE(avl.contains(i));
  }
}

TEST(set, normal_set) {
  std::set<int> std_set;
  std::srand(123);  

  // There should be assertion in invariant check
  // if something goes wrong
  for (int i = 0; i < 100000; ++i) {
    int r = std::rand() % 100000;
    std_set.erase(r);    
    ASSERT_FALSE(std_set.find(r) != std_set.end());
    std_set.insert(r);
    ASSERT_TRUE(std_set.find(r) != std_set.end());
  }
}

TEST(set, random_insertions) {
  avl_set<int> avl;
  std::srand(123);  

  // There should be assertion in invariant check
  // if something goes wrong
  for (int i = 0; i < 100000; ++i) {
    int r = std::rand() % 100000;
    avl.erase(r);    
    ASSERT_FALSE(avl.contains(r));
    avl.insert(r);
    ASSERT_TRUE(avl.contains(r));
  }
}


TEST(persistent_set, use_case) {
  // 1 = {}
  avl_set<double> avl_v1;
  
  // 1 = 2 = {}
  avl_set<double> avl_v2 = avl_v1;
  ASSERT_EQ(0u, avl_v2.size());

  // 1 = {3.1415926} 2 = {}
  avl_v1.insert(3.1415926);
  ASSERT_EQ(1u, avl_v1.size());
  ASSERT_EQ(0u, avl_v2.size());

  // 1 = {3.1415926} 2 = 3 = {}
  avl_set<double>avl_v3 = avl_v2;
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

  ASSERT_TRUE(avl_v1.contains(3.1415926));
  ASSERT_FALSE(avl_v2.contains(3.1415926));
  ASSERT_FALSE(avl_v3.contains(3.1415926));

  ASSERT_TRUE(avl_v1.contains(2.7182818));
  ASSERT_FALSE(avl_v2.contains(2.7182818));
  ASSERT_TRUE(avl_v3.contains(2.7182818));

  // 1 = {3.1415926, 2.7182818} 2 = {3.1415926, 2.7182818, 1.41421356} 3 = {2.7182818}
  avl_v2 = avl_v1;
  avl_v2.insert(1.41421356);
  ASSERT_EQ(2u, avl_v1.size());
  ASSERT_EQ(3u, avl_v2.size());
  ASSERT_EQ(1u, avl_v3.size());

  ASSERT_TRUE(avl_v1.contains(3.1415926));
  ASSERT_TRUE(avl_v2.contains(3.1415926));
  ASSERT_FALSE(avl_v3.contains(3.1415926));

  ASSERT_TRUE(avl_v1.contains(2.7182818));
  ASSERT_TRUE(avl_v2.contains(2.7182818));
  ASSERT_TRUE(avl_v3.contains(2.7182818));

  ASSERT_FALSE(avl_v1.contains(1.41421356));
  ASSERT_TRUE(avl_v2.contains(1.41421356));
  ASSERT_FALSE(avl_v3.contains(1.41421356));
}




#include <gtest/gtest.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <queue>

#include "left_heap.h"

namespace pds {

TEST(left_heap, simple) {
  left_heap<int> h;
  int size = 20;
  for (size_t i = 0; i < size; i++) {
    h.push(i);
  }
  for (size_t i = 0; i < size; i++) {
    ASSERT_EQ(i, h.top());
    h.pop();
  }
  for (size_t i = size; i > 0; i--) {
    h.push(i);
  }
  for (size_t i = 1; i <= size; i++) {
    ASSERT_EQ(i, h.top());
    h.pop();
  }
}

TEST(left_heap, merge_simple) {
  left_heap<int> h1;
  left_heap<int> h2;
  int size = 20;
  for (size_t i = 0; i < size; i++) {
    h1.push(i);
    h2.push(size+i);
  }
  h1.merge(h2);
  for (size_t i = 0; i < size*2; i++) {
    ASSERT_EQ(i, h1.top());
    h1.pop();
  }
  h1.clear();
  h2.clear();
  for (size_t i = size; i > 0; i--) {
    h1.push(i);
    h2.push(size+i);
  }
  h1.merge(h2);
  for (size_t i = 1; i <= size*2; i++) {
    ASSERT_EQ(i, h1.top());
    h1.pop();
  }
}

TEST(left_heap, random_vector) {
  std::vector<int> v;
  int size = 100;
  for (size_t i = 0; i < size; i++) {
    v.push_back(i);
  }
  random_shuffle(v.begin(), v.end());
  left_heap<int> h(v.begin(), v.end());
  for (size_t i = 0; i < size; i++) {
    ASSERT_EQ(i, h.top());
    h.pop();
  }
}

TEST(left_heap, stress_random_vector) {
  int size = 100000;
  for (size_t j = 0; j < 10; j++) {
    std::srand(j);
    std::vector<int> v;
    for (size_t i = 0; i < size; i++) {
      v.push_back(i);
    }
    random_shuffle(v.begin(), v.end());
    left_heap<int> h;
    h.push(v.begin(), v.end());
    for (size_t i = 0; i < size; i++) {
      ASSERT_EQ(i, h.top());
      h.pop();
    }
  }
}

TEST(left_heap, merge_stress_random_vector) {
  int size = 100000;
  for (size_t j = 0; j < 10; j++) {
    std::srand(j);
    std::vector<int> v1,v2;
    for (size_t i = 0; i < size; i++) {
      v1.push_back(i);
      v2.push_back(i);
    }
    random_shuffle(v1.begin(), v1.end());
    random_shuffle(v2.begin(), v2.end());
    left_heap<int> h1,h2;
    h1.push(v1.begin(), v1.end());
    h2.push(v2.begin(), v2.end());
    h1.merge(h2);
    for (size_t i = 0; i < size; i++) {
      ASSERT_EQ(i, h1.top());
      h1.pop();
      ASSERT_EQ(i, h1.top());
      h1.pop();
      ASSERT_EQ(i, h2.top());
      h2.pop();
    }
  }
}

TEST(left_heap, stress_random_vector_priority_queue) {
  int size = 100000;
  for (size_t j = 0; j < 10; j++) {
    std::srand(j);
    std::vector<int> v;
    for (size_t i = 0; i < size; i++) {
      v.push_back(i);
    }
    random_shuffle(v.begin(), v.end());
    std::priority_queue<int, std::vector<int>, std::greater<int> > h(v.begin(), v.end());
    for (size_t i = 0; i < size; i++) {
      ASSERT_EQ(i, h.top());
      h.pop();
    }
  }
}


TEST(left_heap, huge_T_stress_test) {
  int size = 10000;
  int size_T_vector = 100;
  for (size_t j = 0; j < 100; j++) {
    std::srand(j);
    left_heap<std::vector<int> > h;
    std::vector<std::vector<int>> v;
    for (size_t i = 0; i < size; i++) {
      std::vector<int> tmp = std::vector<int>(size_T_vector, i);
      v.push_back(tmp);
    }
    random_shuffle(v.begin(), v.end());
    for (auto i : v) {
      h.push(i);
    }
    for (size_t i = 0; i < size; i++) {
      ASSERT_EQ(std::vector<int>(size_T_vector, i), h.top());
      h.pop();
    }
  }
}


TEST(left_heap, simple_persistence) {
  left_heap<int> h1;
  h1.push(5);
  left_heap<int> h2(h1);
  h2.push(4);
  h2.push(6);

  left_heap<int> h3 = h2;
  ASSERT_EQ(4, h3.top());
  h3.pop();

  h3 = h1;
  ASSERT_EQ(5, h3.top());
  h3.pop();
  ASSERT_EQ(1, h3.empty());

  ASSERT_EQ(5, h1.top());
  h1.pop();
  ASSERT_EQ(1, h1.empty());

  ASSERT_EQ(4, h2.top());
  h2.pop();
  ASSERT_EQ(5, h2.top());
  h2.pop();
  ASSERT_EQ(6, h2.top());
  h2.pop();
  ASSERT_EQ(1, h2.empty());

}

TEST(left_heap, stress_persistence) {
  left_heap<int> h1;
  std::vector<int> v;
  int size = 100;
  for (size_t i = 0; i < size; i++) {
    v.push_back(i);
  }
  random_shuffle(v.begin(), v.end());
  for (auto i : v) {
    h1.push(i);
  }
  // h1 = [0, 1, .. , 99]
  left_heap<int> h2(h1);
  // h2 = [0, 1, .. , 99]
  for (size_t i = 0; i < (size / 2); i++) {
    ASSERT_EQ(i, h2.top());
    h2.pop();
  }
  // h2 = [50, 51, .. ,99]
  for (size_t i = size; i < size * 2; i++) {
    h2.push(i);
  }
  // h2 = [50, 51, .. ,199]
  left_heap<int> h3(h2);
  // h3 = [50, 51, .. ,199]
  for (size_t i = (size / 2); i < (3 * size / 2); i++) {
    ASSERT_EQ(i, h2.top());
    h2.pop();
  }
  // h2 = [150, ... 199]
  for (size_t i = 0; i < size; i++) {
    ASSERT_EQ(i, h1.top());
    h1.pop();
  }
  ASSERT_EQ(1, h1.empty());
  for (auto i : v) {
    h2.push(i);
  }
  //h2 = [0, .. , 99, 150, .. , 199]
  for (size_t i = (size / 2); i < 2 * size; i++) {
    ASSERT_EQ(i, h3.top());
    h3.pop();
  }
  ASSERT_EQ(1, h3.empty());

  for (size_t i = 0; i < size; i++) {
    ASSERT_EQ(i, h2.top());
    h2.pop();
  }

  for (size_t i = 3 * size / 2; i < 2 * size; i++) {
    ASSERT_EQ(i, h2.top());
    h2.pop();
  }
  ASSERT_EQ(1, h2.empty());
}

} // namespace pds

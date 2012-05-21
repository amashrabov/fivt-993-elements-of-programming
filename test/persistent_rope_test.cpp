#include "gtest/gtest.h"

#include "persistent_rope.h"
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <climits>

#include <thread>

namespace pds {

TEST(persistent_rope, simple) {
  srand(time(NULL));
  persistent_rope<int> r;
  ASSERT_EQ(true, r.empty());

  int size = 100;
  for (size_t i = 0; i < size; i++) {
    r.push_back(i);
    ASSERT_EQ(i, r.back());
  }

  r.reverse();

  for (int i = 0; i < size; i++) {
    ASSERT_EQ(size - 1 - i, r[i]);
  }

  r.reverse();

  for (size_t i = 0; i < size; i++) {
    ASSERT_EQ(r.front(), i);
    r.pop_front();
  }

  ASSERT_EQ(true, r.empty());

}


TEST(persistent_rope, random_push) {
  srand(time(NULL));
  int size = 200;
  int num = 1000;

  persistent_rope<int> r;
  std::vector<int> v;

  //TODO : persistent_rope.resize()
  for (size_t i = 0; i < size; i++) {
    r.push_back(0);
    v.push_back(0);
  }
  
  for (int i = 0; i < num; i++) {
    int val = rand() % INT_MAX;
    int pos = rand() % size;
    v[pos] = val;
    r[pos] = val;
  }

  int b = 5;
  int e = 17;
  r.reverse();
  reverse(v.begin(), v.end());

  r.erase(b, e);
  v.erase(v.begin() + b, v.begin() + e);

  for (int i = 0; i < size - e + b; i++) {
    ASSERT_EQ(v[i], r[i]);
  }
}

}

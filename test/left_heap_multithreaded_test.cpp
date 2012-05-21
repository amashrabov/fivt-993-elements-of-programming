#include <gtest/gtest.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <thread>

#include "left_heap.h"

namespace pds {

class left_heap_checker {

 public:

  left_heap_checker(const left_heap<int> heap, std::vector<int> difference) :
      heap_(heap), difference_(difference) {
  }

  left_heap_checker() :
      heap_(), difference_() {
  }

  void operator()(size_t circles, int for_random) {
    srand(for_random);
    for (size_t i = 0; i < circles; ++i) {
      if (rand() % 2 == 0) {
        int a = rand() % difference_.size();
        heap_.push(a);
        ++difference_[a];
      } else {
        if (!heap_.empty()) {
          --difference_[heap_.top()];
          heap_.pop();
        }
      }
    }
  }

  void random_fill(size_t count, int for_random){
    srand(for_random);
    difference_.resize(count);
    for (size_t i = 0; i < count; ++i) {
      int a = rand() % difference_.size();
      heap_.push(a);
      ++difference_[a];
    }

  }

  bool check_and_delete() {
    while (!heap_.empty()) {
      --difference_[heap_.top()];
      heap_.pop();
    }
    bool result = true;
    for (int i : difference_) {
      result &= (i == 0);
    }
    return result;
  }

  left_heap<int> heap_;
  std::vector<int> difference_;

};


TEST(left_heap_multitheaded, simple) {
  left_heap_checker checker;
  checker.random_fill(10000, 0);
  left_heap_checker checker1 = checker;
  left_heap_checker checker2 = checker;
  std::thread th1(checker1, 100000, 1);
  std::thread th2(checker2, 100000, 2);
  th1.join();
  th2.join();
  ASSERT_TRUE(checker.check_and_delete());
  ASSERT_TRUE(checker1.check_and_delete());
  ASSERT_TRUE(checker2.check_and_delete());
}


TEST(left_heap_multitheaded, 10_threads) {
  left_heap_checker checker;
  checker.random_fill(100000, 0);
  std::vector<std::thread> threads(10);
  std::vector<left_heap_checker> checkers(10);
  for (int i = 0; i < 10; ++i) {
    checkers[i] = checker;
    threads[i] = std::thread(checkers[i], 1000000, i);
  }
  for (int i = 0; i < 10; ++i) {
    threads[i].join();
    ASSERT_TRUE(checkers[i].check_and_delete());
  }
  ASSERT_TRUE(checker.check_and_delete());
}

} // namespace pds

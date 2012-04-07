#include <gtest/gtest.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <thread>

#include "persistent_heap.h"

namespace pds {

class heap_cheeker {

 public:

  heap_cheeker(const persistent_heap<int> heap, std::vector<int> difference) :
      heap_(heap), difference_(difference) {
  }

  heap_cheeker() :
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

  bool cheek_and_delete() {
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

  persistent_heap<int> heap_;
  std::vector<int> difference_;

};


TEST(persistent_heap_multitheaded, simple) {
  heap_cheeker cheeker;
  cheeker.random_fill(10000, 0);
  heap_cheeker cheeker1 = cheeker;
  heap_cheeker cheeker2 = cheeker;
  std::thread th1(cheeker1, 100000, 1);
  std::thread th2(cheeker2, 100000, 2);
  th1.join();
  th2.join();
  ASSERT_TRUE(cheeker.cheek_and_delete());
  ASSERT_TRUE(cheeker1.cheek_and_delete());
  ASSERT_TRUE(cheeker2.cheek_and_delete());
}


TEST(persistent_heap_multitheaded, 10_threads) {
  heap_cheeker cheeker;
  cheeker.random_fill(10000, 0);
  std::vector<std::thread> threads(10);
  std::vector<heap_cheeker> cheekers(10);
  for (int i = 0; i < 10; ++i) {
    cheekers[i] = cheeker;
    threads[i] = std::thread(cheekers[i], 100000, i);
  }
  for (int i = 0; i < 10; ++i) {
    threads[i].join();
    ASSERT_TRUE(cheekers[i].cheek_and_delete());
  }
  ASSERT_TRUE(cheeker.cheek_and_delete());
}

} // namespace pds

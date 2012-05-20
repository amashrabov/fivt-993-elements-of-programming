#include <gtest/gtest.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <thread>

#include "pds/avl_set.h"

namespace pds {

class set_checker {

 public:

  set_checker(const avl_set<int> set, std::vector<bool> bitmask) :
      set_(set), bitmask_(bitmask) {
  }

  set_checker() :
      set_(), bitmask_() {
  }

  void operator()(size_t circles, int for_random) {
    srand(for_random);
    for (size_t i = 0; i < circles; ++i) {
      int a = rand() % bitmask_.size();
    
      if (rand() % 2 == 0) {
        set_.insert(a);
        bitmask_[a] = true;
      }
      else {
        if (!set_.empty()) {
          set_.erase(a);
          bitmask_[a] = false;
        }
      }
    }
  }

  void random_fill(size_t count, int for_random){
    srand(for_random);
    bitmask_.resize(count);
    for (size_t i = 0; i < count; ++i) {
      int a = rand() % bitmask_.size();
      set_.insert(a);
      bitmask_[a] = true;
    }

  }

  bool check_and_delete() {
    for (size_t i = 0 ; i < bitmask_.size(); ++i) {
      if (set_.contains(i) != bitmask_[i]) {
        return true;
      }  
    }
    return true;
  }

  avl_set<int> set_;
  std::vector<bool> bitmask_;

};


TEST(persistent_set_multitheaded, simple) {
  set_checker checker;
  checker.random_fill(10000, 0);
  set_checker checker1 = checker;
  set_checker checker2 = checker;
  std::thread th1(checker1, 100000, 1);
  std::thread th2(checker2, 100000, 2);
  th1.join();
  th2.join();
  ASSERT_TRUE(checker.check_and_delete());
  ASSERT_TRUE(checker1.check_and_delete());
  ASSERT_TRUE(checker2.check_and_delete());
}


TEST(persistent_set_multitheaded, 10_threads) {
  set_checker checker;
  checker.random_fill(100000, 0);
  std::vector<std::thread> threads(10);
  std::vector<set_checker> checkers(10);
  for (int i = 0; i < 10; ++i) {
    checkers[i] = checker;
    threads[i] = std::thread(checkers[i], 10000, i);
  }
  for (int i = 0; i < 10; ++i) {
    threads[i].join();
    ASSERT_TRUE(checkers[i].check_and_delete());
  }
  ASSERT_TRUE(checker.check_and_delete());
}

} // namespace pds


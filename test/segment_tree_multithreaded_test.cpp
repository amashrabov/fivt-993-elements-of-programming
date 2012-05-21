#include <gtest/gtest.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <thread>

#include "segment_tree.h"
#include "simple_seg_tree.h"

namespace pds {


typedef PersistantSegmentTree<int, std::plus<int> > T1;
typedef simple_st_version<int, std::plus<int> > T2;

class multithread_stress
{ 
 private:
  T1 tree1_;
  T2 tree2_;

 public:
  

  multithread_stress(const T1 &tree1, const T2 &tree2)
	:tree1_(tree1), tree2_(tree2)
  {}

  void operator() (){
    int size = tree1_.size();
    int kol = 400;
    for (int i = 0; i < kol; ++i)
    {
      int operCode = std::rand() % 2;
      long long w = std::rand() % 3;
      int x = std::rand() % size;
      int y = std::rand() % size;
      if (x > y) std::swap(x,y);
    
      // ASSIGN
      if (operCode)
      {
          tree1_.assign(x, y, w);
          tree2_.assign(x, y, w);
      }
      // ADD
      else {
          tree1_.add(x, y, w);
          tree2_.add(x, y, w);
      }
    } 
  }

  void check() {
    for (size_t j = 0; j < tree1_.size(); ++j)
      for (size_t k = j; k < tree1_.size(); ++k)
        ASSERT_EQ(tree1_.findSum(j, k),
                  tree2_.findSum(j, k));
  }
};

TEST(persistent_seg_tree_multithreaded, 10_threads) {
  std::vector<multithread_stress> stressers; 
  std::vector<std::thread> threads(20);
  multithread_stress base_stresser(T1(100, 0), T2(100, 0));

  for (int i = 0; i < 20; ++i) {
    stressers.push_back(base_stresser);
    threads[i] = std::thread(stressers[i]);
  }
  for (int i = 0; i < 20; ++i) {
    threads[i].join();
    stressers[i].check();
  }
}

}; // namespace pds



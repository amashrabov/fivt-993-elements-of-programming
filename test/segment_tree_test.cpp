#ifndef _SEGMENT_TREE_TEST_H_
#define _SEGMENT_TREE_TEST_H_

#include <functional>
#include <utility>
#include <algorithm>
#include <vector>
#include <gtest/gtest.h>

#include "segment_tree.h"
#include "simple_seg_tree.h"

using namespace pds;

struct Deg
{
  long long operator ()(size_t x, long long a)
  {
    long long ans = 1;
    while (x)
    {
      if (x % 2 == 1)
        ans *= a;
      a *= a;
      x /= 2;
    }
    return ans;
  }
};

std::pair<size_t, size_t> genRandomInterval(size_t size)
{
  size_t l = std::rand()%size;
  size_t r = std::rand()%size;
  if (l > r)
    std::swap(l, r);
  return std::make_pair(l, r);
}

struct Point
{
  int x, y, z;
  Point(){};  
  Point(int x, int y, int z)
  :x(x), y(y), z(z)
  {}
};

Point operator + (const Point &p1, const Point &p2)
{
  return Point(p1.x+p2.x, p1.y+p2.y, p1.z+p2.z);
}

Point operator * (size_t n, const Point &p1)
{
  return Point(n*p1.x, n*p1.y, n*p1.z);
}

bool operator == (const Point &p1, const Point &p2)
{
  return (p1.x == p2.x && p1.y == p2.y && p1.z == p2.z); 
}

typedef PersistantSegmentTree<long long, std::multiplies<long long>, Deg> T1;
typedef simple_st_version<long long, std::multiplies<long long> > T2;

TEST(seg_tree, test_case1)
{
  double x = 0;
  PersistantSegmentTree<double> tree(10, x); // 0 0 0 0 0 0 0 0 0 0
  tree.assign(2, 6, 1.2); // 0 0 1.2 1.2 1.2 1.2 1.2 0 0 0

  ASSERT_EQ(2.4, tree.findSum(5, 7)); 
  ASSERT_EQ(0.0, tree.findSum(1, 1)); 
  ASSERT_EQ(4.8, tree.findSum(2, 5)); 

  tree.add(4, 9, -2); // 0 0 1.2 1.2 -0.8 -0.8 -0.8 -2 -2 -2
  ASSERT_TRUE(fabs(tree.findSum(2, 5) - 0.8) < 1e-9); 
}
TEST(seg_tree, test_case2)
{
  PersistantSegmentTree<Point> tree(23, Point(0, 1, 2));
  PersistantSegmentTree<Point> tree1 = tree;

  tree.add(4, 10, Point(-2, -1, 0));
  tree1.assign(1, 12, Point(0, 0, 3));

  simple_st_version<Point> str(23, Point(0, 1, 2));
  simple_st_version<Point> str1 = str;

  str.add(4, 10, Point(-2, -1, 0));
  str1.assign(1, 12, Point(0, 0, 3));

  ASSERT_EQ(tree.findSum(2, 7), str.findSum(2, 7));
  ASSERT_EQ(tree1.findSum(2, 7), str1.findSum(2, 7));
} 
TEST(seg_tree, random_stress_test)
{
  int size = 1000;
  std::vector<T1> tree1(1, T1(size, (long long)0));
  std::vector<T2> tree2(1, T2(size, (long long)0));

  for (int i = 0; i < 10000; ++i)
  {
    int num = rand() % tree1.size();
    tree1.push_back(tree1[num]);
    tree2.push_back(tree2[num]);
    auto p = genRandomInterval(size);
    int operCode = rand() % 3;    
    long long w = rand() % 1000 - 500;

    switch (operCode) {
      // Assign
      case 0 :
        tree1.back().assign(p.first, p.second, w);
        tree2.back().assign(p.first, p.second, w);
        break;
      // Add
      case 1 :
        tree1.back().add(p.first, p.second, w);
        tree2.back().add(p.first, p.second, w);
        break;
      // Test for equality
      case 2 :
        ASSERT_EQ(tree1.back().findSum(p.first, p.second),
                  tree2.back().findSum(p.first, p.second));
    }
  }
}

#endif

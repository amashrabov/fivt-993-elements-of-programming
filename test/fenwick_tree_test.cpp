#include <gtest/gtest.h>

#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;
using std::swap;

#include "fenwick_tree.h"

TEST (FenwickTreeTest, SimpleTest1D) {
  typedef std::array<size_t,1> Coord;
  Coord c,c1;
  c[0] = 6;
  stlext::FenwickTree<1,int> tree(c);
  int a[]={4,8,7,1,6,5};
  for (int i = 0; i < 6; ++i) {
    c[0] = i;
    tree.set(c,a[i]);
  }
  //for(int i=0;i<6;++i){
  //  cout<<tree.data_[i]<<" ";
  //}
  //cout<<endl;
  for(int i = 0; i < 6; ++i) {
    c[0] = i;
    int x = tree.get(c);
    EXPECT_EQ(a[i],x);
  }
  c[0] = 2; c1[0] = 5;
  EXPECT_EQ(14, tree.aggregate(c,c1));
}

TEST (FenwickTreeTest, RandomTest1D) {
  srand(865986);
  typedef std::array<size_t,1> Coord;
  Coord c,c1;
  const size_t N = 2345;
  int arr[N];
  c[0] = N;
  stlext::FenwickTree<1,int> tree(c);
  for(int i=0;i<N;++i){
    c[0]=i;
    arr[i]=rand();
    tree.set(c,arr[i]);
  }
  for(int i=0;i<N;++i){
    c[0]=i;
    EXPECT_EQ(arr[i],tree.get(c));
  }
  for(size_t i=0;i<N*20;++i) {
    size_t a = rand()%(N+1);
    size_t b = rand()%(N+1);
    if(a>b) {
      swap(a,b);
    }
    int sum = 0;
    for(size_t j = a; j < b; ++j) {
      sum += arr[j];
    }
    c[0] = a;
    c1[0] = b;
    EXPECT_EQ(sum,tree.aggregate(c,c1));
  }
}

TEST (FenwickTreeTest, SimpleTest2D) {
  typedef std::array<size_t,2> Coord;
  Coord c,c1;
  int arr[][5]={{1,2,3,4,5},{6,7,8,9,10},{11,12,13,14,15},{16,17,18,19,20}};
  c[0] = 4;
  c[1] = 5;
  stlext::FenwickTree<2,int> tree(c);
  //cout<<tree.koefs_[0]<<endl;
  //cout<<tree.koefs_[1]<<endl;
  for(int i=0;i<4;++i){
    for(int j=0;j<5;++j){
      c[0]=i; c[1]=j;
      tree.set(c,arr[i][j]);
    }
  }
  for(int i=0;i<4;++i){
    for(int j=0;j<5;++j){
      c[0]=i;
      c[1]=j;
      EXPECT_EQ(arr[i][j],tree.get(c));
    }
  }
  c[0] = 0;
  c[1] = 0;
  c1[0] = 4;
  c1[1] = 5;
  EXPECT_EQ(210,tree.aggregate(c,c1));
}

TEST(FenwickTreeTest, RandomTest2D) {
  srand(23490);
  typedef std::array<size_t,2> Coord;
  Coord c,c1;
  const size_t N = 141;
  const size_t M = 463;
  vector<vector<int> > arr(N);
  for(size_t i = 0; i < N; ++i){
    arr[i].resize(M);
    for(size_t j = 0; j < M; ++j) {
      arr[i][j] = rand() - RAND_MAX/2;
    }
  }
  c[0] = N;
  c[1] = M;
  stlext::FenwickTree<2,int> tree(c);
  for(size_t i = 0; i < N; ++i){
    for(size_t j = 0; j < M; ++j) {
      c[0] = i;
      c[1] = j;
      tree.set(c,arr[i][j]);
    }
  }
  for(size_t i = 0; i < N; ++i){
    for(size_t j = 0; j < M; ++j) {
      c[0] = i;
      c[1] = j;
      EXPECT_EQ(arr[i][j],tree.get(c));
    }
  }
  for(size_t k = 0; k < N*M; ++k){
    //cout<<k<<endl;
    c[0] = rand()%(N+1);
    c[1] = rand()%(M+1);
    c1[0] = rand()%(N+1);
    c1[1] = rand()%(M+1);
    if(c[0]>c1[0]){
      swap(c[0],c1[0]);
    }
    if(c[1]>c1[1]){
      swap(c[1],c1[1]);
    }
    int sum = 0;
    for(size_t i = c[0]; i < c1[0]; ++i){
      for(size_t j = c[1]; j < c1[1]; ++j){
        sum+=arr[i][j];
      }
    }
    EXPECT_EQ(sum,tree.aggregate(c,c1));
  }
}

#ifndef _FENWICK_TREE_H_
#define _FENWICK_TREE_H_

#include <algorithm>
#include <vector>

#include <cassert>
#include <cstdarg>

using std::vector;

/*
Author: Alex Taran
Module: Fenwick tree
Description: 
        To be done
Notes:
        Coords [x1,x2,x3, ... xN]
        Sizes  [a1,a2,a3, ... aN]
        Counting of them: x1*a2*..*aN + x2*a3*a4..*aN + ... + x(N-1)*aN + xN
*/

namespace stlext {

template<size_t N, class T, class AddFunc,class SubFunc>
  class FenwickTree {
   public:
    typedef vector<size_t> Coord;

    FenwickTree<N, T, AddFunc, SubFunc> () : dims_(N, 0){
      reset();
    }

    FenwickTree<N, T, AddFunc, SubFunc> (const Coord& dims) : dims_(dims) {
      assert(dims.size() == N);
      reset();
    }
    /*
    T& operator [] (const Coord& c) {
      return getElement(c);
    }

    const T& operator[] (const Coord& c) const {
      return getElement(c);
    }
    */
    Coord size() const {
      return dims_;
    }

    T aggregate(const Coord& c1,const Coord& c2) const {
      assert(c1.size() == N);
      assert(c2.size() == N);
      for(size_t i = 0; i < N; ++i) {
        assert(c1[i]<=c2[i]);
      }
      // TODO: Some street magic returning the sum of elements
    }

    void set(const Coord& c,const T& value){
      assert(c.size() == N);
      // TODO: Some street magic which does the job
    }

    T get(const Coord& c){
      asserd(c.size() == N);
      Coord c1 = c;
      for (size_t i = 0; i < N; ++i) {
        c1[i]++;
      }
      return aggregate(c,c1);
    }
   private:
    Coord dims_;
    Coord koefs_;
    vector<T> data_;

    AddFunc addFunc_;
    SubFunc subFunc_;

    static size_t power (size_t a, size_t p) {
      size_t res = 1;
      for (size_t i = 0; i < p; ++i) {
        res *= a;
      }
      return res;
    }

    void reset () { /* resets koefs_ and data_ size*/
      size_t k = 1, i = N;
      do {
        --i;
        koefs_[i] = k;
        k *= dims_[i];
      } while (i > 0);
      data_.resize(k);
    }

    T& getElement(const Coord& c) { /* gets element by coordinate*/
      assert(c.size() == N);
      size_t k = 0;
      for (size_t i = 0; i < N; ++i) {
        k += koefs_[i] * c[i];
      }
      assert(k < data_.size());
      return data_[k];
    }
  };
}   // namespace stlext

#endif  // _FENWICK_TREE_H_

// Copyright 2012 Alex Taran
#ifndef STLEXT_FENWICK_TREE_H_
#define STLEXT_FENWICK_TREE_H_

#include <array>
#include <functional>
#include <vector>

#include <cassert>
#include <cstddef>
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

template<size_t N, class T,
  class AddFunc = std::plus<T>,
  class SubFunc = std::minus<T> >
  class FenwickTree {
   public:
    typedef std::array<size_t, N> Coord;

    FenwickTree<N, T, AddFunc, SubFunc> () {
      dims_.fill(0);
      reset();
    }

    FenwickTree<N, T, AddFunc, SubFunc> (const Coord& dims) : dims_(dims) {
      reset();
    }

    const T& operator[] (const Coord& c) const {
      return get(c);
    }

    Coord size() const {
      return dims_;
    }

    T aggregate(const Coord& c) const {
      Coord curr = c;
      T result = getPrevElement(c);
      while (true) {
        // iterate for 'curr'
        size_t i;
        for (i = N - 1; i > 0; --i) {
          if ((curr[i] & (curr[i] + 1))  <= 1) {
            curr[i] = c[i];
          } else {
            break;
          }
        }
        curr[i] = (curr[i] & (curr[i] + 1));
        if (curr[0] == 0) {
          break;
        }
        curr[i]--;
        // aggregate a result
        result = addFunc_(result, getPrevElement(curr));
      }
      return result;
    }

    T aggregate(const Coord& c1, const Coord& c2) const {
      for (size_t i = 0; i < N; ++i) {
        assert(c1[i] <= c2[i]);
      }
      // I/E formula here
      Coord c = c2;
      T result = aggregate(c);
      bool negation = false;
      for (size_t i = 1; i < (1<<N); ++i) {  // bitmask
        size_t gray = i ^ (i >> 1);
        negation = !negation;
        for (size_t j = 0; j < N; ++j) {  // replace bits
          if ((gray >> j) & 1 == 1) {
            c[j] = c1[j];
          } else {
            c[j] = c2[j];
          }
        }
        if (negation) {
          result = subFunc_(result, aggregate(c));
        } else {
          result = addFunc_(result, aggregate(c));
        }
      }
      return result;
    }

    void set(const Coord& c, const T& value) {
      Coord curr = c;
      T delta = subFunc_(value, get(c));
      do {
        incElement(curr, delta);
        size_t i;
        for (i = N - 1; i > 0; --i) {
          if (curr[i] | (curr[i] + 1) >= dims_[i]) {
            curr[i] = c[i];
          } else {
            break;
          }
        }
        curr[i] = curr[i] | (curr[i] + 1);
        if (curr[0] >= dims_[0]) {
          break;
        }
      } while (true);
    }

    T get(const Coord& c) const {
      Coord c1 = c;
      for (size_t i = 0; i < N; ++i) {
        c1[i]++;
      }
      return aggregate(c, c1);
    }

   private:
    Coord dims_;
    Coord koefs_;
    std::vector<T> data_;

    AddFunc addFunc_;
    SubFunc subFunc_;
    T neutralElem_;

    void reset() { /* resets koefs_ and data_ size*/
      size_t k = 1, i = N;
      do {
        --i;
        koefs_[i] = k;
        k *= dims_[i];
      } while (i > 0);
      data_.resize(k);
      neutralElem_ = subFunc_(data_[0], data_[0]);
    }

    const T& getElement(const Coord& c) const { /* gets element by coordinate*/
      size_t k = 0;
      for (size_t i = 0; i < N; ++i) {
        k += koefs_[i] * c[i];
      }
      assert(k < data_.size());
      return data_[k];
    }

    const T& getPrevElement(const Coord& c) const {
      size_t k = 0;
      for (size_t i = 0; i < N; ++i) {
        if (c[i] == 0) {
          return neutralElem_;
        }
        k += koefs_[i] * (c[i]-1);
      }
      assert(k < data_.size());
      return data_[k];
    }

    void incElement(const Coord& c, const T& value) {
      size_t k = 0;
      for (size_t i = 0; i < N; ++i) {
        k += koefs_[i] * c[i];
      }
      assert(k < data_.size());
      data_[k] = addFunc_(data_[k], value);
    }

    static bool isZeroCoord(const Coord& c) {
      for (size_t i = 0; i < N; ++i) {
        if (c[i] != 0) {
          return false;
        }
      }
      return true;
    }
  };
}   // namespace stlext

#endif  // STLEXT_FENWICK_TREE_H_

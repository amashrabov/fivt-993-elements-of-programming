#ifndef _FIND_FIRST_NOT_LESS_H_
#define _FIND_FIRST_NOT_LESS_H_

#include <functional>
#include <algorithm>
#include <iterator>
#include <iostream>
#include "comparator_mutator.h"

// Function returns Iterator to the first element in 
// the [begin, end) not compared less than given element.
// If no such element is found returns end.
// Function's output is quite the same as lower_bound 
template<class Iterator, class T, class Comparator>
Iterator find_first_not_less(Iterator begin, Iterator end, const T& element, Comparator less) {
  Iterator it(begin);
  Iterator it_next(it);
  Iterator temp;
  
  greater_or_equal<Comparator> not_less(less);  
  
  typename std::iterator_traits<Iterator>::difference_type step = 1;
  typename std::iterator_traits<Iterator>::difference_type avaliable_distance;


  // Checking for extreme caseslf lf 
  if (std::distance(begin, end) <= 0) return end;
  if (not_less(*begin, element)) return begin;
  
  avaliable_distance = std::distance(it_next, end);


  // ALOGORITHM'S STEP 1
  //
  // After this one it refers to an object < than element
  // and it_next to an object compared >= than element or
  // if no such to the end.
  //
  // Also note that after this part of algorithm step > (it_next, it]

  // Each iteration of the cycle doubles the distance between
  // it & it_next till we find a proper pair of iterators (see
  // comment above) or next iteration would make one of
  // iterators out of range.
  while (avaliable_distance > step) {
    if (not_less(*it_next, element))
      break;

    it = it_next;
    it_next += step;
    avaliable_distance -= step;
    step <<= 1;
  }

  // One more step may be required due to step is always 
  // a power of 2.
  if (less(*it_next, element)) {
    it = it_next;
    it_next = end;
  }


  // ALOGITHM'S STEP 2
  //
  // Searching on the (it, it_next]. After this one we get the answer.

  // Now this variable maintains relevant distance between it_next and it
  avaliable_distance = it_next - it;

  // Each iteration 0 <= avaliable_distance <= 2 * step and step is twice
  // less than on previous iteration. 
  // Iterator it referes to an object compared less than element
  while (step != 0) {
    if (avaliable_distance > step) {
      temp = it + step;
      if (less(*temp, element)) {
        it = temp;
        avaliable_distance -= step;
      }
    }
    step >>= 1;
  } 
  
  // Cause it is the last element compared less than element
  // returning ++it
  return ++it;
}

template <class Iterator, class T>
Iterator find_first_not_less(Iterator begin, Iterator end, const T& element) {
  return find_first_not_less(begin, end, element, std::less<T>());
}

#endif

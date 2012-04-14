#ifndef _FIND_FIRST_NOT_LESS_H_
#define _FIND_FIRST_NOT_LESS_H_

#include <functional>

namespace stlext {

// Function returns Iterator to the first element in 
// the [begin, end) not compared less than given element.
// If no such element is found returns end.
// Function's output is quite the same as lower_bound 
template<class Iterator, class T, class Comparator>
Iterator find_first_not_less(Iterator begin, Iterator end, const T& element, const Comparator& less) {
  // This variable is used to make test steps.
  // Thou in part A and part B the steps made
  // are different - the main idea remains.
  Iterator it_next(begin);

  // The size of the following step.
  // Used both in A and B parts of the algorithm.
  typename std::iterator_traits<Iterator>::difference_type step = 1;

  // The size of an interval we are interested in at the moment.
  typename std::iterator_traits<Iterator>::difference_type avaliable_distance = std::distance(begin, end);

  // The only thing algorithm uses is that begin <= end.
  // If this is wrong - exiting.
  if (avaliable_distance <= 0)
    return end;

  // ALOGORITHM'S PART A 
  
  // In this part  we decrease the distance of [begin, end)
  // so that it is still a valid search interval.
  // Here 'avaliable_distance' represents the distance of
  // [it_next, end) but after this cycle it will represent
  // the distance of [begin, end).
  while (true) {
    // Here we make sure that 'begin' can be made 'it_next'
    // so that [begin, end) would be still a valid search
    // interval. If not - [begin, it_next) is the last valid.
    if (!less(*it_next, element)) {
      end = it_next;
      step >>= 1;
      avaliable_distance = step;
      break;
    }
 
    // Making step forward. Which means begin = it_next,
    // it_next = it_next + step.
    // 'begin' is guaranteed to be valid after this but 
    // it_next may become an out of range Iterator.
    begin = it_next;

    // So here we check if 'it_next' would become fine
    // after incrementation. If yes - there is no point
    // in incrementing it. [begin, end) is the last valid.
    if (avaliable_distance <= step) {
      avaliable_distance = end - begin;
      break;
    }
    
    avaliable_distance -= step;
    it_next += step;
    step <<= 1;
  }

  // Invariant check.
  assert(avaliable_distance == std::distance(begin, end));

  // ALOGITHM'S PART B
  //
  // Searching on the [begin, end). From the end.
  // Trying to get as close to begin as we can.
  while (step != 0) {
    if (avaliable_distance > step) {
      it_next = end - step;
      if (!less(*it_next, element)) {
        end = it_next;
	avaliable_distance -= step;
      }
    }
    step >>= 1;
  }

  return end;
}

template <class Iterator, class T>
Iterator find_first_not_less(Iterator begin, Iterator end, const T& element) {
  return find_first_not_less(begin, end, element, std::less<T>());
}

};
#endif

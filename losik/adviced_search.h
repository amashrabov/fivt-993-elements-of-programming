#ifndef _ADVICED_SEARCH_H_
#define _ADVICED_SEARCH_H_

#include <iterator>
#include <functional>
#include <utility>
#include "comparator_mutator.h"
#include "find_first_not_less.h"

// Sarches for the first object compared not less than element in the sorted [begin, end) interval.
// Returns end if no such element present.
// The time is O(log|advice - result|)
template <class Iterator, class T, class Comparator>
Iterator adviced_lower_bound(Iterator begin, Iterator end, Iterator advice, const T& element, Comparator compare) {
  // Empty or broken interval case
  if (std::distance(begin, end) <= 0) return end;
  // Advice is out of bonds cases
  if (std::distance(advice, end) <= 0) advice = end - 1;
  if (std::distance(begin, advice) < 0) advice = begin;

  // Deciding where to search - left or right of the advice.
  if (compare(*advice, element)) {
    // This function is perfectly apt for this perpose
    return find_first_not_less(advice, end, element, compare);
  }
  else {
    // Reversing the interval and so reversing the comparator.
    // This one finds the last element, compared < on the
    // (begin, advice] interval. Then after reversing the iterator
    // back it is shifted one right so it refers to the first
    // element compared not less than the one searched.
    return find_first_not_less(
      std::reverse_iterator<Iterator>(advice),
      std::reverse_iterator<Iterator>(begin),
      element,
      create_greater_or_equal(compare)).base();
  }
}

template <class Iterator, class T>
Iterator adviced_lower_bound(Iterator begin, Iterator end, Iterator advice, const T& element) {
  return adviced_lower_bound(begin, end, advice, element, std::less<T>());
}

// Searches for the frst element compared greater than "element".
template <class Iterator, class T, class Comparator>
Iterator adviced_upper_bound(Iterator begin, Iterator end, Iterator advice, const T& element, Comparator compare) {
  // The first element not compared as <= is the one compared >.
  return adviced_lower_bound(begin, end, advice, element, create_less_or_equal(compare));
}

template <class Iterator, class T>
Iterator adviced_upper_bound(Iterator begin, Iterator end, Iterator advice, const T& element) {
  return adviced_upper_bound(begin, end, advice, element, std::less<T>());
}

// Returns the (lower_bound, upper_bound)
template <class Iterator, class T, class Comparator>
std::pair<Iterator, Iterator> adviced_equal_range(Iterator begin, Iterator end, Iterator advice, const T& element, Comparator compare) {
  Iterator first(begin), last(end);  

  // Empty or broken interval case
  if (std::distance(begin, end) <= 0) return std::make_pair(end, end);
  // Advice is out of bonds cases
  if (std::distance(advice, end) <= 0) advice = end - 1;
  if (std::distance(begin, advice) < 0) advice = begin;
  

  // Knowing one of the bounds we can optimise the advice
  // and the search interval to find the other one quicker.
  
  // begin <= advice < element
  //
  // Both searces are straightforward. Having found lower bound
  // we use it as the beginning of the interval for the upper bound search.
  if (compare(*advice, element)) {
    first = find_first_not_less(advice + 1, end, element, compare);
    last = find_first_not_less(first + 1, end, element, create_less_or_equal(compare));
  }
  
  // element < advice < end
  //
  // Both searches here are performed in revesed direction.
  // This means that instead normal comparator the reversed one should be used.
  // See more detailed explanation in adviced_lower_bound comments.
  else if (compare(element, *advice)) {
    // In lower_bound "<" would normally be used, so we reverse it to ">"
    last = find_first_not_less(std::reverse_iterator<Iterator>(advice),
                               std::reverse_iterator<Iterator>(begin),
                               element,
                               create_greater(compare)).base();
    // In upper_bound "<=" would normally be used, so we reverse it to ">="
    first = find_first_not_less(std::reverse_iterator<Iterator>(last - 1),
                                std::reverse_iterator<Iterator>(begin),
                                element,
                                create_greater_or_equal(compare)).base();
  }
  
  // begin <= element == advice < end
  //
  // Here one search is reversed and the other is straightforward
  // and the other is reversed.
  else {
    first = find_first_not_less(std::reverse_iterator<Iterator>(advice),
                                std::reverse_iterator<Iterator>(begin),
				element,
				create_greater_or_equal(compare)).base();            
    last = find_first_not_less(advice + 1, end, element, create_less_or_equal(compare));
  }
       
  return std::make_pair(first, last);
}

template <class Iterator, class T>
std::pair<Iterator, Iterator> adviced_equal_range(Iterator begin, Iterator end, Iterator advice, const T& element) {
  return adviced_equal_range(begin, end, advice, element, std::less<T>());
}

#endif

#ifndef _COMPARATOR_MUTATOR_H_
#define _COMPARATOR_MUTATOR_H_

#include <functional>

namespace stlext {

// Comparator wrapper class 
template<class Comparator>
class less: public std::binary_function<typename Comparator::first_argument_type,
                                        typename Comparator::second_argument_type,
                                        bool> {
 public:
  explicit less(Comparator compare): compare_(compare) {}
  
  inline bool operator() (typename Comparator::first_argument_type first,
                          typename Comparator::second_argument_type second) const  {
    return compare_(first, second);
  }

 private:
  Comparator compare_;
};


// Comparator wrapper class
// Converts template "<" comparator to ">" comparator
template<class Comparator>
class greater: public std::binary_function<typename Comparator::first_argument_type,
                                           typename Comparator::second_argument_type,
                                           bool> {
 public:
  explicit greater(Comparator compare): compare_(compare) {
  }
  
  inline bool operator() (typename Comparator::first_argument_type first,
                          typename Comparator::second_argument_type second) const {
    return compare_(second,first);
  }

 private:
  Comparator compare_;
};


// Comparator wrapper class
// Converts template "<" comparator to "<=" comparator
template<class Comparator>
class less_or_equal: public std::binary_function<typename Comparator::first_argument_type,
                                                 typename Comparator::second_argument_type,
                                                 bool> {
 public:
  explicit less_or_equal(Comparator compare): compare_(compare) {
  }
  
  inline bool operator() (typename Comparator::first_argument_type first,
                          typename Comparator::second_argument_type second) const {
    return !compare_(second,first);
  }

 private:
  Comparator compare_;
};


// Comparator wrapper class
// Converts template "<" comparator to ">=" comparator
template<class Comparator>
class greater_or_equal: public std::binary_function<typename Comparator::first_argument_type,
                                                    typename Comparator::second_argument_type,
                                                    bool> {
 public:
  explicit greater_or_equal(Comparator compare): compare_(compare) {
  }
  
  inline bool operator() (typename Comparator::first_argument_type first,
                          typename Comparator::second_argument_type second) const {
    return !compare_(first, second);
  }

 private:
  Comparator compare_;
};


// Comparator wrapper class
// Converts template "<" comparator to "==" comparator
template<class Comparator> 
class equal: public std::binary_function<typename Comparator::first_argument_type,
                                         typename Comparator::second_argument_type,
                                         bool> {
 public:
  explicit equal(Comparator compare): compare_(compare) {
  }
  
  inline bool operator() (typename Comparator::first_argument_type first,
                          typename Comparator::second_argument_type second) const {
    return !(compare_(first, second) || compare_(second,first));
  }

 private:
  Comparator compare_;
};


// Comparator wrapper class
// Cpnverts template "<" comparator to "!=" comparator
template<class Comparator> 
class not_equal: public std::binary_function<typename Comparator::first_argument_type,
                                             typename Comparator::second_argument_type,
                                             bool> {
 public:
  explicit not_equal(Comparator compare): compare_(compare) {
  }
  
  inline bool operator() (typename Comparator::first_argument_type first,
                          typename Comparator::second_argument_type second) const {
    return compare_(second,first) || compare_(first, second);
  }

 private:
  Comparator compare_;
};



// Returns new comparator object with the same operation as given
template <class Comparator>
less<Comparator> create_less(Comparator compare) {
  return less<Comparator>(compare);
}

// Considering parameter "<" comparator returns ">" comparator object
template <class Comparator>
greater<Comparator> create_greater(Comparator compare) {
  return greater<Comparator>(compare);
}

// Considering parameter "<" comparator returns "<=" comparator object
template <class Comparator>
less_or_equal<Comparator> create_less_or_equal(Comparator compare) {
  return less_or_equal<Comparator>(compare);
}

// Considering parameter "<" comparator returns ">=" comparator object
template <class Comparator>
greater_or_equal<Comparator> create_greater_or_equal(Comparator compare) {
  return greater_or_equal<Comparator>(compare);
}

// Considering parameter "<" comparator returns "==" comparator object
template <class Comparator>
equal<Comparator> create_equal(Comparator compare) {
  return equal<Comparator>(compare);
}

// Considering parameter "<" comparator returns "!=" comparator object
template <class Comparator>
not_equal<Comparator> create_not_equal(Comparator compare) {
  return not_equal<Comparator>(compare);
}

}; // stlext

#endif

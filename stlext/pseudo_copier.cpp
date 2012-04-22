#ifndef _PSEUDO_COPIER_CPP_
#define _PSEUDO_COPIER_CPP_

class PseudoCopier {
 public:
  template<class T>
  static T& copy(const T &value) {
     return const_cast<T&>(value);
  } 
};

#endif

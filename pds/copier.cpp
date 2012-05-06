#ifndef _COPIER_CPP_
#define _COPIER_CPP_

class Copier {
 public:
  template<class T>
  static T copy(const T &value) {
     return T(value);
  } 
};

#endif

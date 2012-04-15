#include <memory>
#include "avl_tree_node.hpp"

namespace stlext {

template<class T>
class AvlSet {
 public:
  typedef typename AvlTreeNode<T>::node_ptr   node_ptr;
  typedef typename AvlTreeNode<T>::node_cptr  node_cptr;
  typedef typename AvlTreeNode<T>::node_sptr  node_sptr;
  typedef typename AvlTreeNode<T>::node_csptr node_csptr;

  AvlSet(): size_(0) {}
  bool insert(const T &value);
  void remove(const T &value);
  bool contains(const T &value) const;

  size_t size() const;


 private:
  node_csptr root_;
  size_t size_;
};

};

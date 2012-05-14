#ifndef _AVL_TREE_NODE_HPP_
#define _AVL_TREE_NODE_HPP_

#include <memory>
#include <gtest/gtest.h>
#include "pds_ptr.h"

namespace pds {
  
template<class T>
struct avl_tree_node {
  typedef avl_tree_node<T>* node_ptr;
  typedef avl_tree_node<T>* const node_cptr;
  typedef pds_ptr<avl_tree_node<T> > node_sptr;
  typedef pds_ptr<const avl_tree_node<T> > node_csptr;

  template <class ... Args>
  avl_tree_node(Args... args):
    value_(args...),
    height_(1) {}

  avl_tree_node(const avl_tree_node<T>& node):
    value_(node.value_),
    height_(node.height_) {
    child_[0] = node.child_[0];
    child_[1] = node.child_[1];
  }

  int height_;
  T value_;
  node_csptr child_[2];
};

}; // namespace pds

#endif //_AVL_TREE_NODE_HPP_

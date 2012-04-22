#include <memory>
#include <cmath>
#include <iostream>
#include <gtest/gtest.h>
#include "copier.cpp"

namespace stlext {
  
template<class T, class Comparator>
class AvlTreeNode {
 public:

  typedef AvlTreeNode<T, Comparator>* node_ptr;
  typedef AvlTreeNode<T, Comparator>* const node_cptr;
  typedef std::shared_ptr<AvlTreeNode<T, Comparator> > node_sptr;
  typedef std::shared_ptr<const AvlTreeNode<T, Comparator> > node_csptr;

  template <class ... Args>
  AvlTreeNode(Args... args):
    value_(args...),
    height_(1),
    compare_() {}

  AvlTreeNode(const AvlTreeNode<T, Comparator>& node):
    value_(node.value_),
    height_(node.height_),
    compare_() {
    child_[0] = node.child_[0];
    child_[1] = node.child_[1];
  }

  void find_and_remove(node_csptr &ref, const T &value, bool &remove);
  const T& get_value() const;

  // PRIVATE ? O_O

  node_csptr& find(node_csptr &ref, const T &value);
  
  int neg(int i, bool b);

  // Const functions
  static int get_height(const node_csptr &node);
  static int get_factor(const node_csptr &node);

  // Balance related functions
  void update_height(node_csptr &ref);  
  void balance(const node_csptr &ref, bool inverse = false);
  void small_rotation(node_csptr &ref, bool inverse = false);
  void big_rotation(node_csptr &ref, bool inverse = false);

  // Removal related functions
  void remove_rec(node_csptr &ref, T &val, int dir);

  // Debug related functions
  void invariant_check(const node_csptr &ref) const;


  int height_;
  T value_;
  node_csptr child_[2];
  Comparator compare_;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T, class Comparator>
const T& AvlTreeNode<T, Comparator>::get_value() const {
  return value_;
}

// Reverts the last bit of given number
template<class T, class Comparator>
int AvlTreeNode<T, Comparator>::neg(int i, bool b) {
  if (b)
    i ^= 0x01;

  return i;
}

template<class T, class Comparator>
void AvlTreeNode<T, Comparator>::invariant_check(const node_csptr &ref) const {
  EXPECT_LT(get_factor(ref), 2);
  EXPECT_GT(get_factor(ref), -2);
}






// REMOVAL RELATED SECTION


// Considering subtree a valid AVL tree
// removes node with given value if present.
// Reference to subtree, passed through
// ref is updated.
// If sucessfully removed - remove is true
// remove is false otherwise.
template<class T, class Comparator>
void AvlTreeNode<T, Comparator>::find_and_remove(node_csptr &ref, const T &value, bool &remove) {
  // We are in the node we want to delete
  remove = false;
  if (value_ == value) {
    if (height_ == 1) {
      remove = true;
      // After this command our node may be deleted
      ref = node_csptr();
      return;
    }
    else {
      // FIXME What a terrible failure!
      
      int index = (get_factor(ref) > 0) ? 0 : 1;
      int dir =   (get_factor(ref) > 0) ? 1 : 0;
      auto& child = child_[index];
      remove = true;
      const_cast<node_ptr>(child.get())->remove_rec(child, value_, dir);
    }
  }
  else {
    size_t index = (compare_(value, value_))? 0 : 1;
    //auto& child = child_[index];
   
    if (child_[index].get() != NULL)
      const_cast<node_ptr>(child_[index].get())->find_and_remove(child_[index], value, remove);
  }

  balance(ref);
  
}

// Considering subtree a valid AVL tree
// implements a recursive part of removal
// algorithm.
// Reference to subtree, passed through
// ref is updated.
template<class T, class Comparator>
void AvlTreeNode<T, Comparator>::remove_rec(node_csptr &ref, T &val, int dir) {
  int child_index = dir;
  
  // If no further movement in given direction
  // is possible - we must perform some actions
  if (child_[dir].get() == NULL) {
    // pushing up the value
    val = std::move(value_);

    // we can be at the bottom of recursion
    if (height_ == 1) {
      ref = node_csptr();
      return;
    }
    else {
      auto& child = const_cast<node_ptr>(ref.get())->child_[neg(dir, true)];
      value_ = std::move(const_cast<node_ptr>(child.get())->value_);
      child = node_csptr();
    }
  }
  else {
    const_cast<node_ptr>(child_[dir].get())->remove_rec(child_[dir], val, dir);
  }

  balance(ref);
}


// BALANCE RELATED SECTION

template<class T, class Comparator>
int AvlTreeNode<T, Comparator>::get_height(const node_csptr &node) {
  return (node.get() == NULL)? 0 : node->height_;
}

template<class T, class Comparator>
int AvlTreeNode<T, Comparator>::get_factor(const node_csptr &node) {
  if (node.get() == NULL)
    return 0;

  return get_height(node->child_[0]) - get_height(node->child_[1]);
}

template<class T, class Comparator>
void AvlTreeNode<T, Comparator>::update_height(node_csptr &ref) {
  if (ref.get() == NULL)
    return;

  int l_h = get_height(ref->child_[0]);
  int r_h = get_height(ref->child_[1]);
  int new_height = std::max(l_h, r_h) + 1;
  
  if (ref->height_ != new_height) {
    ref = Copier::copy(ref);
    const_cast<node_ptr>(ref.get())->height_ = new_height;
  }
}

template<class T, class Comparator>
void AvlTreeNode<T, Comparator>::balance(const node_csptr &ref, bool inverse) {
  node_csptr& nonconst_ref = const_cast<node_csptr&>(ref);
  update_height(nonconst_ref);

  if (get_factor(nonconst_ref) <= -2) {
    if (get_factor(child_[1]) <= 0) {
      small_rotation(nonconst_ref);
    }
    else {
      big_rotation(nonconst_ref);
    } 
  }
  else if (get_factor(nonconst_ref) >= 2) {
    if (get_factor(child_[0]) >= 0) {
      small_rotation(nonconst_ref, true);
    }
    else {
      big_rotation(nonconst_ref, true);
    } 
  }  
}

// root             root
//   |               |
//   a               b  
//  / \             / \
// L   \    ==>    a   \
//      b         / \   R
//     / \       L   C
//    C   R
// L, C, R nodes don't change
// a, b nodes do change
template<class T, class Comparator>
void AvlTreeNode<T, Comparator>::small_rotation(node_csptr &ref, bool inverse) {

  // root        root  a_tmp     
  //   |            \ /  
  //   a             a   
  //  / \           / \  b_tmp
  // L   \    ==>  L   \ /
  //      b             b
  //     / \           / \
  //    C   R         C   R

  auto a_tmp = Copier::copy(ref);
  auto b_tmp = Copier::copy(ref->child_[neg(1, inverse)]);

  //root a_tmp     a_tmp     
  //  \ /            |  
  //   a             a   
  //  / \  b_tmp    / \   root
  // L   \ /  ==>  L   C  /
  //      b              b
  //     / \            / \
  //    C   R          C   R
  const_cast<node_ptr>(a_tmp.get())->child_[neg(1, inverse)] = b_tmp->child_[neg(0, inverse)];
  ref = b_tmp;

  // a_tmp            root
  //   |                |
  //   a                b
  //  / \   root ==>   / \
  // L   C  /         a   R
  //       b         / \
  //      / \       L   C
  //     ?   R
  const_cast<node_ptr>(ref.get())->child_[neg(0, inverse)] = a_tmp;
 
  // Updating heights
  update_height(const_cast<node_ptr>(ref.get())->child_[neg(0, inverse)]);
  update_height(ref);
}


template<class T, class Comparator>
void AvlTreeNode<T, Comparator>::big_rotation(node_csptr &ref, bool inverse) {
  // root        root  a_tmp     
  //   |            \ /  
  //   a             a   
  //  / \           / \  b_tmp
  // L   \    ==>  L   \ /
  //      b             b
  //     / \    c_tmp  / \
  //    c   R       \ /   R
  //   / \           c 
  //  M   N         / \
  //               M   N
  auto a_tmp = Copier::copy(ref);
  auto b_tmp = Copier::copy(ref->child_[inverse? 0 : 1]);
  auto c_tmp = Copier::copy(b_tmp->child_[inverse? 1 : 0]);

  //  root a_tmp         a_tmp 
  //     \ /               |
  //      a                a
  //     / \  b_tmp       / \     b_tmp
  //    L   \ /          L   M      |
  //         b      ==>             b
  // c_tmp  / \              root  / \
  //     \ /   R               |  N   R
  //      c                    c
  //     / \                  / \
  //    M   N                M   N
  
  // root_child is a now
  const_cast<node_ptr>(a_tmp.get())->child_[neg(1, inverse)] = c_tmp->child_[neg(0, inverse)];
  const_cast<node_ptr>(b_tmp.get())->child_[neg(0, inverse)] = c_tmp->child_[neg(1, inverse)];
  ref = c_tmp;

  //   a_tmp                  root
  //     |                     |
  //     a                     c
  //    / \   b_tmp           / \
  //   L   M    |    ==>     /   \
  //            b           a     b
  //     root  / \         / \   / \
  //       |  N   R       L   M N   R
  //       c
  //      / \
  //     M   N
  const_cast<node_ptr>(ref.get())->child_[inverse? 1 : 0] = a_tmp;
  const_cast<node_ptr>(ref.get())->child_[inverse? 0 : 1] = b_tmp;

  // Updating heights
  update_height(const_cast<node_ptr>(ref.get())->child_[inverse? 1 : 0]);
  update_height(const_cast<node_ptr>(ref.get())->child_[inverse? 0 : 1]);
  update_height(ref);
}

}; // namespace stlext


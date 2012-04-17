#include <memory>
#include <cmath>
#include <iostream>
#include <gtest/gtest.h>

namespace stlext {

// TODO consider replacing NULL with nullptr.
// nullptr is not yet present in GCC 4.5 (the 
// latest stable release of GCC for Gentoo users).

  
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

  node_csptr& find(node_csptr &ref, const T &value);
  bool contains(const node_csptr &ref, const T &value) const;
  bool insert(node_csptr &ref, const T &value);
  void find_and_remove(node_csptr &ref, const T &value, bool &remove);

 private:
  int neg(int i, bool b);

  // Const functions
  int get_height(const node_csptr &node) const;
  int get_factor(const node_csptr &node) const;

  // Balance related functions
  void update_height(const node_csptr &ref);  
  void balance(node_csptr &ref, bool inverse = false);
  void small_rotation(node_csptr &ref, bool inverse = false);
  void big_rotation(node_csptr &ref, bool inverse = false);

  // Removal related functions
  void remove_rec(node_csptr &ref, T &val, int dir);

  // Debug related functions
  void invariant_check(const node_csptr &ref) const;
  
  Comparator compare_;
  int height_;
  T value_;
  node_csptr child_[2];
};




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




// SEARCH RELATED SECTION


// Considering subtree a valid AVL tree
// returns true if value is present in 
// this subtree and false otherwise.
template<class T, class Comparator>
bool AvlTreeNode<T, Comparator>::contains(const node_csptr &ref, const T &value) const {
  invariant_check(ref);

  if (value_ == value)
    return true;

  size_t index = (compare_(value, value_))? 0 : 1;
  auto& child = child_[index];
  
  if (child.get() == NULL)
    return false;

  return child->contains(child, value);
}


// INSERTION RELATED SECTION


// Considering subtree a valid AVL tree
// inserts new node with given value.
// Reference to subtree, passed through
// ref is updated.
// Returns true if new is element was
// inserted, false otherwise.
template<class T, class Comparator>
bool AvlTreeNode<T, Comparator>::insert(node_csptr &ref, const T &value) {
  if (value_ == value)
    return false;

  size_t index = (compare_(value, value_))? 0 : 1;
  auto& child = child_[index];
  bool ans;

  if (child.get() == NULL) {
    child = std::make_shared<const AvlTreeNode<T, Comparator> >(value);
    ans = true;  
  }
  else {
    ans = const_cast<node_ptr>(child.get())->insert(child, value);
  }

  balance(ref);
  invariant_check(ref);

  return ans;

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
int AvlTreeNode<T, Comparator>::get_height(const node_csptr &node) const {
  return (node.get() == NULL)? 0 : node->height_;
}

template<class T, class Comparator>
int AvlTreeNode<T, Comparator>::get_factor(const node_csptr &node) const {
  if (node.get() == NULL)
    return 0;

  return get_height(node->child_[0]) - get_height(node->child_[1]);
}

template<class T, class Comparator>
void AvlTreeNode<T, Comparator>::update_height(const node_csptr &ref) {
  if (ref.get() == NULL)
    return;

  int l_h = get_height(ref->child_[0]);
  int r_h = get_height(ref->child_[1]);
  const_cast<node_ptr>(ref.get())->height_ = std::max(l_h, r_h) + 1;
}

template<class T, class Comparator>
void AvlTreeNode<T, Comparator>::balance(node_csptr &ref, bool inverse) {
  update_height(ref);

  if (get_factor(ref) <= -2) {
    if (get_factor(child_[1]) <= 0) {
      small_rotation(ref);
    }
    else {
      big_rotation(ref);
    } 
  }
  else if (get_factor(ref) >= 2) {
    if (get_factor(child_[0]) >= 0) {
      small_rotation(ref, true);
    }
    else {
      big_rotation(ref, true);
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
  auto a_tmp = ref;
  auto b_tmp = ref->child_[neg(1, inverse)];

  //root a_tmp     a_tmp     
  //  \ /            |  
  //   a             a   
  //  / \  b_tmp    / \   root
  // L   \ /  ==>  L   C  /
  //      b              b
  //     / \            / \
  //    C   R          ?   R
  const_cast<node_ptr>(ref.get())->child_[neg(1, inverse)] = std::move(b_tmp->child_[neg(0, inverse)]);
  ref = std::move(b_tmp);

  // a_tmp            root
  //   |                |
  //   a                b
  //  / \   root ==>   / \
  // L   C  /         a   R
  //       b         / \
  //      / \       L   C
  //     ?   R
  const_cast<node_ptr>(ref.get())->child_[neg(0, inverse)] = std::move(a_tmp);
 
  // Updating heights
  update_height(ref->child_[neg(0, inverse)]);
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
  auto a_tmp = ref;
  auto b_tmp = ref->child_[inverse? 0 : 1];
  auto c_tmp = b_tmp->child_[inverse? 1 : 0];

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
  //    M   N                ?   ?
  
  // root_child is a now
  const_cast<node_ptr>(ref.get())->child_[neg(1, inverse)] = std::move(c_tmp->child_[neg(0, inverse)]);
  const_cast<node_ptr>(b_tmp.get())->child_[neg(0, inverse)] = std::move(c_tmp->child_[neg(1, inverse)]);
  ref = std::move(c_tmp);

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
  //     ?   N
  const_cast<node_ptr>(ref.get())->child_[inverse? 1 : 0] = std::move(a_tmp);
  const_cast<node_ptr>(ref.get())->child_[inverse? 0 : 1] = std::move(b_tmp);

  // Updating heights
  update_height(ref->child_[inverse? 1 : 0]);
  update_height(ref->child_[inverse? 0 : 1]);
  update_height(ref);
}

}; // namespace stlext


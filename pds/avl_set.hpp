#ifndef _AVL_SET_HPP_
#define _AVL_SET_HPP_

#include <memory>
#include <functional>
#include "avl_tree_node.hpp"
#include "pseudo_copier.cpp"

namespace stlext {

template<class T, class Comparator = std::less<T> , class NodeCopier = PseudoCopier>
class avl_set {
 public:
  typedef avl_tree_node<T> node_t;
  typedef typename node_t::node_ptr   node_ptr;
  typedef typename node_t::node_cptr  node_cptr;
  typedef typename node_t::node_sptr  node_sptr;
  typedef typename node_t::node_csptr node_csptr;


  avl_set(): 
    size_(0) {}

  avl_set(const avl_set<T, Comparator, NodeCopier> &set):
    size_(set.size_),
    root_(set.root_) {}
 
  bool insert(const T &value);
  bool remove(const T &value);
  bool contains(const T &value) const;

  size_t size() const;

 private:
  bool insert(const T &value, node_csptr &ref);
  bool remove(const T &value, node_csptr &ref);
  void replace(node_csptr &ref, T &value, int dir);

  void balance(node_csptr &ref);
  void small_rotation(node_csptr &ref, bool inverse = false);
  void big_rotation(node_csptr &ref, bool inverse = false);
  void update_height(node_csptr &ref);  

  int get_factor(const node_csptr &node) const;
  int get_height(const node_csptr &node) const;

  int neg(int i, bool inverse) {
    return inverse? i ^ 0x01 : i;
  }

  node_csptr root_;
  size_t size_;
  Comparator compare_;
};

template<class T, class Comparator, class NodeCopier>
int avl_set<T, Comparator, NodeCopier>::get_height(const node_csptr &node) const {
  return (node.get() == NULL)? 0 : node->height_;
}

template<class T, class Comparator, class NodeCopier>
int avl_set<T, Comparator, NodeCopier>::get_factor(const node_csptr &node) const {
  if (node.get() == NULL)
    return 0;

  return get_height(node->child_[0]) - get_height(node->child_[1]);
}


template<class T, class Comparator, class NodeCopier>
bool avl_set<T, Comparator, NodeCopier>::contains(const T& value) const {
  auto node = root_;

  while (true) {
    if (node.get() == NULL)
      return false;

    if (node->value_ == value)
      return true;
 
    size_t index = (compare_(value, node->value_))? 0 : 1;
    node = node->child_[index];    
  }
}

template<class T, class Comparator, class NodeCopier>
bool avl_set<T, Comparator, NodeCopier>::insert(const T &value) {
  return insert(value, root_);
}



template<class T, class Comparator, class NodeCopier>
bool avl_set<T, Comparator, NodeCopier>::insert(const T &value, node_csptr &ref) {
  // The place to insert is reached
  // Creationg new leaf here.
  if (ref.get() == NULL) {
    node_csptr& nonconst_ref = const_cast<node_csptr&>(ref);
    nonconst_ref = std::make_shared<const node_t>(value);
    ++size_;
    return true;
  }
    
  // The value already exists. No insertions are needed.
  if (ref->value_ == value)
    return false;
  
  size_t index = (compare_(value, ref->value_))? 0 : 1;
  
  // After insertion child_cpy will contain new 
  // root of child's subtree.
  node_csptr child_cpy = ref->child_[index];
  bool answ = insert(value, child_cpy);
  // If new child's root changed we must make a
  // copy of this node and change reference to
  // this subtree.
  if (child_cpy != ref->child_[index]) {
    // Copying the node
    // TODO do we need copier here if new shared pointer
    // is constructed anyways?
    ref = std::make_shared<const node_t>(NodeCopier::copy(*ref));
    // Updating the node
    const_cast<node_ptr>(ref.get())->child_[index] = child_cpy;
    // Balancing subtree of the node
    balance(ref);
  }

  return answ;
}


template<class T, class Comparator, class NodeCopier>
bool avl_set<T, Comparator, NodeCopier>::remove(const T& value) {
  if (root_.get() == NULL) {
    return false;
  }

  bool ans = remove(value, root_);

  if (ans) {
    --size_;
  }

  return ans;
}

template<class T, class Comparator, class NodeCopier>
bool avl_set<T, Comparator, NodeCopier>::remove(const T &value, node_csptr &ref) {
  // We are in the node we want to delete
  // So we delete it.
  if (ref->value_ == value) {
    // If this is a list - everything is
    // very simple. Just deleting it.
    if (ref->height_ == 1) {
      ref = node_csptr();
    }
    // If this is not a list - a substitution
    // for this node is needed. Searching for
    // this substitution is initiated.
    else {  
      int index = (get_factor(ref) > 0) ? 0 : 1;
      int dir = index ^ 0x01;
      // TODO the first time writing like this. If anything happens - check this line
      node_t new_node = NodeCopier::copy(*ref);

      replace(new_node.child_[index], new_node.value_, dir);
      ref = std::make_shared<node_t>(new_node);
      balance(ref);
    }

    return true;
  }

  // The node to delete is not yet found
  // So continuing search.
  bool answ = false;
  size_t index = (compare_(value, ref->value_))? 0 : 1; 
  if (ref->child_[index].get() != NULL) {
    node_csptr child_cpy = ref->child_[index];
    answ = remove(value, child_cpy);

    // Checking if we have to update current 
    if (child_cpy != ref->child_[index]) {
      node_t new_node = NodeCopier::copy(*ref);
      new_node.child_[index] = child_cpy;
      ref = std::make_shared<node_t>(new_node);
    }
  }

  balance(ref);
  return answ;
}


template<class T, class Comparator, class NodeCopier>
void avl_set<T, Comparator, NodeCopier>::replace(node_csptr &ref, T &value, int dir) {
  int child_index = dir;

  // If no further movement in given direction
  // is possible - we must perform some actions
  if (ref->child_[dir].get() == NULL) {
    // The value is not needed here
    value = std::move(ref->value_);

    // If the replacement is a leaf
    // just forgetting about it.
    if (ref->height_ == 1) {
      ref = node_csptr();
    }
    // If this is not a leaf then the 
    // node only has one child.We put
    // this child into parentr's place.
    else {
      // This reverts the last bit of dir.
      // It converts 1 to 0 and 0 to 1.
      ref = ref->child_[dir ^= 0x01];
    }

    // Everything we wanted to do is done
    return;
  }

  // Going further till we reach the bottom.
  // We have to update all the nodes cause
  // replacing element will always be deleted.
  node_t new_node = NodeCopier::copy(*ref);
  replace(new_node.child_[dir], value, dir);
  ref = std::make_shared<node_t>(new_node);
  balance(ref);
}


template<class T, class Comparator, class NodeCopier>
void avl_set<T, Comparator, NodeCopier>::balance(node_csptr &ref) {
  update_height(ref);

  if (get_factor(ref) <= -2) {
    if (get_factor(ref->child_[1]) <= 0) {
      small_rotation(ref);
    }
    else {
      big_rotation(ref);
    } 
  }
  else if (get_factor(ref) >= 2) {
    if (get_factor(ref->child_[0]) >= 0) {
      small_rotation(ref, true);
    }
    else {
      big_rotation(ref, true);
    } 
  }  
}

template<class T, class Comparator, class NodeCopier>
void avl_set<T, Comparator, NodeCopier>::update_height(node_csptr &ref) {
  if (ref.get() == NULL)
    return;

  int l_h = get_height(ref->child_[0]);
  int r_h = get_height(ref->child_[1]);
  int new_height = std::max(l_h, r_h) + 1;
  
  if (ref->height_ != new_height) {
    node_t new_node = NodeCopier::copy(*ref);
    new_node.height_ = new_height;
    ref = std::make_shared<const node_t>(new_node);
  }
}



template<class T, class Comparator, class NodeCopier>
size_t avl_set<T, Comparator, NodeCopier>::size() const {
  return size_;
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
template<class T, class Comparator, class NodeCopier>
void avl_set<T, Comparator, NodeCopier>::small_rotation(node_csptr &ref, bool inverse) {

  // root        root  a_tmp     
  //   |            \ /  
  //   a             a   
  //  / \           / \  b_tmp
  // L   \    ==>  L   \ /
  //      b             b
  //     / \           / \
  //    C   R         C   R

  auto a_tmp = NodeCopier::copy(ref);
  auto b_tmp = NodeCopier::copy(ref->child_[neg(1, inverse)]);

  //root a_tmp     a_tmp     
  //  \ /            |  
  //   a             a   
  //  / \  b_tmp    / \   root
  // L   \ /  ==>  L   C  /
  //      b              b
  //     / \            / \
  //    C   R          C   R
  const_cast<node_csptr&>(a_tmp->child_[neg(1, inverse)]) = b_tmp->child_[neg(0, inverse)];
  ref = b_tmp;

  // a_tmp            root
  //   |                |
  //   a                b
  //  / \   root ==>   / \
  // L   C  /         a   R
  //       b         / \
  //      / \       L   C
  //     ?   R
  const_cast<node_csptr&>(ref->child_[neg(0, inverse)]) = a_tmp;
 
  // Updating heights
  update_height(const_cast<node_csptr&>(ref.get()->child_[neg(0, inverse)]));
  update_height(ref);
}


template<class T, class Comparator, class NodeCopier>
void avl_set<T, Comparator, NodeCopier>::big_rotation(node_csptr &ref, bool inverse) {
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
  auto a_tmp = NodeCopier::copy(ref);
  auto b_tmp = NodeCopier::copy(ref->child_[inverse? 0 : 1]);
  auto c_tmp = NodeCopier::copy(b_tmp->child_[inverse? 1 : 0]);

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
  const_cast<node_csptr&>(a_tmp->child_[neg(1, inverse)]) = c_tmp->child_[neg(0, inverse)];
  const_cast<node_csptr&>(b_tmp->child_[neg(0, inverse)]) = c_tmp->child_[neg(1, inverse)];
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
  const_cast<node_csptr&>(ref->child_[inverse? 1 : 0]) = a_tmp;
  const_cast<node_csptr&>(ref->child_[inverse? 0 : 1]) = b_tmp;

  // Updating heights
  update_height(const_cast<node_csptr&>(ref->child_[inverse? 1 : 0]));
  update_height(const_cast<node_csptr&>(ref->child_[inverse? 0 : 1]));
  update_height(ref);
}


};// stlext

#endif //_AVL_SET_HPP_

#ifndef _AVL_SET_HPP_
#define _AVL_SET_HPP_

#include <memory>
#include <functional>
#include "avl_tree_node.h"

namespace pds {


template<class T, class Comparator = std::less<T> >
class avl_set {
 public:

  typedef avl_tree_node<T> node_t;
  typedef typename node_t::node_ptr   node_ptr;
  typedef typename node_t::const_node_ptr const_node_ptr;

  avl_set(): 
    size_(0) {}

  avl_set(const avl_set<T, Comparator> &set):
    size_(set.size_),
    root_(set.root_) {}
 
  // Returns true if element was inserted,
  // false if element was already present.
  bool insert(const T &value);

  // Returns true if element was erased,
  // false if element was not present.
  bool erase(const T &value);

  // Returns true if element is present,
  // false otherwise.
  bool contains(const T &value) const;

  // Returns the number of elements.
  size_t size() const;

  // Retuns if set is empty
  bool empty() const;

 private:
  // Recursive insertion function. Returns true
  // if after insertion ref subtree was changed
  // false othrwise. If  element already exists
  // and force == true element will be overwritten.
  bool insert(const T &value, const_node_ptr &ref, bool force = false);

  // erases the value from ref subtree.
  // Returns true if elemnt was erased,
  // false if element was not present.
  bool erase(const T &value, const_node_ptr &ref);

  // A part of removal algorithm. Finds
  // substitution for erased element
  // and swaps the values. 
  void replace(const_node_ptr &ref, T &value, int dir);

  // Consideing ref's both right and left
  // subtrees valid AVL trees balances
  // ref according to AVL rule.
  void balance(const_node_ptr &ref);

  // A part of balancing algorithm.
  void small_rotation(const_node_ptr &ref, bool inverse = false);

  // A part of balancing algorith.
  void big_rotation(const_node_ptr &ref, bool inverse = false);

  // Consideing ref's both right and left
  // subtrees valid AVL trees updates ref's
  // height.
  void update_height(const_node_ptr &ref);  

  int get_factor(const const_node_ptr &node) const;
  int get_height(const const_node_ptr &node) const;

  inline int neg(int i, bool inverse = true) {
    return inverse? i ^ 0x01 : i;
  }

  const_node_ptr root_;
  size_t size_;
  Comparator compare_;
};

template<class T, class Comparator>
int avl_set<T, Comparator>::get_height(const const_node_ptr &node) const {
  return (node.get() == nullptr)? 0 : node->height_;
}

template<class T, class Comparator>
int avl_set<T, Comparator>::get_factor(const const_node_ptr &node) const {
  if (node.get() == nullptr)
    return 0;

  return get_height(node->child_[0]) - get_height(node->child_[1]);
}


template<class T, class Comparator>
bool avl_set<T, Comparator>::contains(const T& value) const {
  auto node = root_;

  while (true) {
    if (node.get() == nullptr)
      return false;

    if (node->value_ == value)
      return true;
 
    size_t index = (compare_(value, node->value_))? 0 : 1;
    node = node->child_[index];    
  }
}


template<class T, class Comparator>
bool avl_set<T, Comparator>::insert(const T &value) {
  return insert(value, root_);
}


template<class T, class Comparator>
bool avl_set<T, Comparator>::insert(const T &value, const_node_ptr &ref, bool force) {
  // Insertion
  if (ref.get() == nullptr) {
    ref = new node_t(value);
    ++size_;
    return true;
  }
    
  // The value already exists
  if (ref->value_ == value) {
    if (force)
      ref.switch_to_mutable()->value_ = std::move(T(value));

    return force;
  }
  
  size_t index = compare_(value, ref->value_)? 0 : 1;
  const_node_ptr child_cpy = ref->child_[index];

  // If new child's subtree has changed
  if (insert(value, child_cpy)) {
    ref.switch_to_mutable()->child_[index] = std::move(child_cpy);
    balance(ref);
    return true;
  }

  return false;
}


template<class T, class Comparator>
bool avl_set<T, Comparator>::erase(const T& value) {
  if (root_.get() == nullptr)
    return false;

  bool ans;

  if (ans = erase(value, root_))
    --size_;

  return ans;
}

template<class T, class Comparator>
bool avl_set<T, Comparator>::erase(const T &value, const_node_ptr &ref) {
  // Node to erase
  if (ref->value_ == value) {
    // List case
    if (ref->height_ == 1) {
      ref.reset();
    }
    // Substitution seach
    else {  
      int index = (get_factor(ref) > 0) ? 0 : 1;
      node_ptr ref_mut = ref.switch_to_mutable();
      replace(ref_mut->child_[index], ref_mut->value_, neg(index));
      balance(ref);
    }

    return true;
  }

  // Continue search
  size_t index = (compare_(value, ref->value_))? 0 : 1; 

  if (ref->child_[index].get() != nullptr) {
    const_node_ptr child_cpy = ref->child_[index];

    // Checking if we have to update current
    if (erase(value, child_cpy)) {
      ref.switch_to_mutable()->child_[index] = std::move(child_cpy);
      balance(ref);
      return true;
    }
  }

  return false;
}


template<class T, class Comparator>
void avl_set<T, Comparator>::replace(const_node_ptr &ref, T &value, int dir) {
  int child_index = dir;

  // No more moving in given direction possible
  if (ref->child_[dir].get() == nullptr) {
    value = std::move(ref->value_);
    // If leaf - deleting this node, else replacing
    // by th only child.
    ref = (ref->height_ == 1)? const_node_ptr() : 
                               ref->child_[neg(dir)];
    return;
  }

  replace(ref.switch_to_mutable()->child_[dir], value, dir);
  balance(ref);
}


template<class T, class Comparator>
void avl_set<T, Comparator>::balance(const_node_ptr &ref) {
  if (ref.get() == nullptr)
    return;

  update_height(ref);
  int factor = get_factor(ref);

  if (factor < -1) {
    if (get_factor(ref->child_[1]) > 0) {
      big_rotation(ref);
    }
    else {
      small_rotation(ref);
    } 
  }
  else if (factor > 1) {
    if (get_factor(ref->child_[0]) < 0) {
      big_rotation(ref, true);
    }
    else {
      small_rotation(ref, true);
    } 
  }  
}


template<class T, class Comparator>
void avl_set<T, Comparator>::update_height(const_node_ptr &ref) {
  int l_h = get_height(ref->child_[0]);
  int r_h = get_height(ref->child_[1]);
  int new_height = std::max(l_h, r_h) + 1;
  
  if (ref->height_ != new_height) {
    ref.switch_to_mutable()->height_ = new_height;
  }
}


template<class T, class Comparator>
size_t avl_set<T, Comparator>::size() const {
  return size_;
}


template<class T, class Comparator>
bool avl_set<T, Comparator>::empty() const {
  return (size_ == 0);
}


// (ref)           (ref)
//   |               |
//   a               b  
//  / \             / \
// L   \    ==>    a   \
//      b         / \   R
//     / \       L   C
//    C   R
template<class T, class Comparator>
void avl_set<T, Comparator>::small_rotation(const_node_ptr &ref, bool inverse) {

  // (ref)        (a_tmp)
  //   |             | 
  //   a             a   
  //  / \           / \ (b_tmp)
  // L   \    ==>  L   \ /
  //      b             b
  //     / \           / \
  //    C   R         C   R
  node_ptr a = ref.switch_to_mutable();
  auto a_tmp = ref;
  
  node_ptr b = a->child_[neg(1, inverse)].switch_to_mutable();
  auto b_tmp = a->child_[neg(1, inverse)];

  //(a_tmp)      (a_tmp)(b_tmp)
  //   |            \     /
  //   a             a   b
  //  / \ (b_tmp)   / \ / \
  // L   \ /  ==>  L   C  R
  //      b              
  //     / \            
  //    C   R          
  a->child_[neg(1, inverse)] = std::move(b->child_[neg(0, inverse)]);

  //(a_tmp)(b_tmp)   (b_tmp)
  //  \     /           |
  //   a   b    (a_tmp) b
  //  / \ / \        \ / \
  // L   C   R  ==>   a   R
  //                 / \
  //                L   C
  b->child_[neg(0, inverse)] = std::move(a_tmp);
 
  // Updating heights
  update_height(b->child_[neg(0, inverse)]);
  update_height(b_tmp);
  ref = b_tmp;
}

//   a             c   
//  / \           / \ 
// L   \    ==>  /   \
//      b       a     b
//     / \     / \   / \
/    c   R    /  \  /  \
//   / \     L   M N   R
//  M   N         
template<class T, class Comparator>
void avl_set<T, Comparator>::big_rotation(const_node_ptr &ref, bool inverse) {
  //              (c_tmp)
  //                 |  
  //   a             a   
  //  / \           / \ (b_tmp)
  // L   \    ==>  L   \ /
  //      b             b
  //     / \   (c_tmp) / \
  //    c   R       \ /   R
  //   / \           c 
  //  M   N         / \
  //               M   N
  node_ptr a = ref.switch_to_mutable();
  auto a_tmp = ref;

  node_ptr b = a->child_[inverse? 0 : 1].switch_to_mutable();
  auto b_tmp = a->child_[inverse? 0 : 1];

  node_ptr c = b->child_[inverse? 1 : 0].switch_to_mutable();
  auto c_tmp = b->child_[inverse? 1 : 0];

  //   (a_tmp)         (a_tmp) 
  //      |               |
  //      a               a
  //     / \ (b_tmp)     / \     (b_tmp)
  //    L   \ /         L   M      |
  //         b      ==>            b

  // (c_tmp)/ \          (c_tmp)  / \
  //     \ /   R            |    N   R
  //      c                 c
  //     / \               / \
  //    M   N             M   N
  a->child_[neg(1, inverse)] = std::move(c->child_[neg(0, inverse)]);
  b->child_[neg(0, inverse)] = std::move(c->child_[neg(1, inverse)]);
 
  //  (a_tmp)                (c_tmp)
  //     |                     |
  //     a                     c
  //    / \    (b_tmp)        / \
  //   L   M      |    ==>   /   \
  //              b         a     b
  //    (c_tmp)  / \       / \   / \
  //       |    N   R     L   M N   R
  //       c
  //      / \
  //     M   N
  c->child_[neg(0, inverse)] = std::move(a_tmp);
  c->child_[neg(1, inverse)] = std::move(b_tmp);

  // Updating heights
  update_height(c->child_[neg(0, inverse)]);
  update_height(c->child_[neg(1, inverse)]);
  update_height(c_tmp);
  ref = c_tmp;
}


};// pds

#endif //_AVL_SET_HPP_

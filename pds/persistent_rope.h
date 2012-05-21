#ifndef PRESISTENT_ROPE_H__
#define PRESISTENT_ROPE_H__

#include "pds_ptr.h"

#include <assert.h>
#include <algorithm>
#include <climits>
#include <functional>
#include <memory>
#include <tuple>
#include <iostream>

namespace pds { // Persistent Data Structures

namespace impl { //implementation

template <class T>
struct node{

  explicit node (T new_value = T())
    : value(new_value), size(1), left(), right(), should_be_reversed() {
    priority = std::rand();
    if (RAND_MAX == 32767) {
      priority += (std::rand() << 16);
    }
  }

  ~node() {}

  unsigned int priority;
  T value;
  pds_ptr<node<T> > left; // left child
  pds_ptr<node<T> > right; // right child
  size_t size; // size of tree where [node] is a root
  bool should_be_reversed;
};

template <class T>
size_t size_of_node(const node<T> * node) {
  if (!node) return 0;
  return node->size;
}

template <class T>
void update_size(node<T> * node) {
  if (node == NULL) {
    return;
  }
  node->size = 1;
  if (node->left.get()) {
    node->size += node->left->size;
  }
  if (node->right.get()) {
    node->size += node->right->size;
  }
}

//if [current_node] should be reversed it will be reversed
template<class T>
pds_ptr<node<T> > check_reversing(pds_ptr<node<T> > current_node) {
  if (!current_node.get() || !current_node->should_be_reversed) {
    return current_node;
  }
  pds_ptr<node<T> > result = current_node;
  node<T> * result_ptr = result.switch_to_mutable();
  node<T> * left_ptr = result_ptr->left.switch_to_mutable();
  node<T> * right_ptr = result_ptr->right.switch_to_mutable();

  std::swap(result_ptr->left, result_ptr->right);
  if (left_ptr) {
    left_ptr->should_be_reversed = true;
  }
  if (right_ptr) {
    right_ptr->should_be_reversed = true;
  }
  result_ptr->should_be_reversed = false;
  return result;
}

//search in subtree of [node] the element on position [pos]
template<class T>
pds_ptr<node<T> > search_in_node_subtree(pds_ptr<node<T> > current_node, size_t pos) {
  current_node = check_reversing(current_node);

  if (!current_node.get()) {
    return NULL;
  }
  size_t current_pos = size_of_node<T>(current_node->left.get());
  if (current_pos < pos) {
    return search_in_node_subtree<T>(current_node->right, pos - current_pos - 1);
  }
  if (current_pos > pos) {
    return search_in_node_subtree<T>(current_node->left, pos);
  }
  return current_node;
}

template<class T>
pds_ptr<node<T> > merge_nodes (pds_ptr<node<T> > left, pds_ptr<node<T> > right) {
  left = check_reversing<T>(left);
  right = check_reversing<T>(right);
  if (!left.get()) {
    return right;
  }
  if (!right.get()) {
    return left;
  }

  pds_ptr<node<T> > result;
  if (impl::rand_less(left->priority, right->priority)) {
    result = right;
    node<T> * result_ptr = result.switch_to_mutable();
    result_ptr->left = merge_nodes(left, right->left);
    update_size<T>(result_ptr);
  } else {
    result = left;
    node<T> * result_ptr = result.switch_to_mutable();
    result_ptr->right = merge_nodes(left->right, right);
    update_size<T>(result_ptr);
	}
	return result;
}

// it splits the subtree of [current_node] by [pos]
// result_1 contains first ([pos] - 1) elements of [current_node]
// result_2 contains remain elements of [current_rope]
template<class T>
std::pair<pds_ptr<node<T> >, pds_ptr<node<T> > > split_node (pds_ptr<node<T> > current_node, size_t pos) {
  current_node = check_reversing<T>(current_node);
  if (!current_node.get()) {
    return std::make_pair(pds_ptr<node<T> >(), pds_ptr<node<T> >());
  }

  size_t current_pos = size_of_node<T>(current_node->left.get());

  pds_ptr<node<T> > left;
  pds_ptr<node<T> > right;
  pds_ptr<node<T> > split;
  if (current_pos < pos) {
    std::tie(split, right) = split_node<T>(current_node->right, pos - current_pos - 1);
    left = current_node;
    node<T> * left_ptr = left.switch_to_mutable();
    left_ptr->right = split;
    update_size<T>(left_ptr);
  } else {
    std::tie(left, split) = split_node<T>(current_node->left, pos);
    right = current_node;
    node<T> * right_ptr = right.switch_to_mutable();
    right_ptr->left = split;
    update_size<T>(right_ptr);
  }
  return std::make_pair(left, right);
}

bool rand_less(const int priority_1, const int priority_2) {
  if (priority_1 == priority_2) {
    return (std::rand() % 2);
  }
  return (priority_1 < priority_2);
}

} //namespace impl

//preface: rand() is used here
template<class T>
class persistent_rope {
public:
  persistent_rope() : root_() {}
  
  explicit persistent_rope(const T &value) : root_(new impl::node<T>(value)) {}

  class reference {
  public:

    operator T() const {
      return (*static_cast<const persistent_rope *>(owner_))[index_];
    }

    reference & operator = (const T& new_value) {
      owner_->insert(index_, new_value);
      return *this;
    }

  private:

    reference(persistent_rope<T> * owner, size_t index)
      : owner_(owner), index_(index) {}

    persistent_rope<T> * owner_;
    size_t index_; 

    friend class persistent_rope;
  };

  
  //inserts [rope] inside of our rope
  //[pos] is the position of received rope where [rope] begins
  bool insert(size_t pos, const persistent_rope &rope) {
    if (pos > this->size()) {
      return false;
    }
    pds_ptr<impl::node<T> > left;
    pds_ptr<impl::node<T> > right;
    std::tie(left, right) = impl::split_node<T>(root_, pos);
    left = impl::merge_nodes<T>(left, rope.root_);
    root_ = impl::merge_nodes<T>(left, right);
    return true;
  }

  bool insert(size_t pos, T value) {
    return this->insert(pos, persistent_rope<T>(value));
  }

  //add [rope] in the end of our rope
  void push_back(const persistent_rope &rope){
    root_ = impl::merge_nodes<T>(root_, rope.root_);
  }

  void push_back(T value) {
    this->push_back(persistent_rope<T>(value));
  }

  //delete elements with positions in [[pos_begin], [pos_end])
  bool erase(size_t pos_begin, size_t pos_end){
    if (pos_begin >= pos_end || pos_end > this->size()) {
      return false;
    }
    pds_ptr<impl::node<T> > left;
    pds_ptr<impl::node<T> > medium;
    pds_ptr<impl::node<T> > right;

    std::tie(left, right) = impl::split_node<T>(root_, pos_begin);
    std::tie(medium, right) = impl::split_node<T>(right, pos_end);
    root_ = impl::merge_nodes<T>(left, right);
    return true;
  }

  bool erase(size_t pos) {
    return this->erase(pos, pos + 1);
  }

  void pop_back() {
    this->erase(this->size() - 1);
  }

  void pop_front() {
	  this->erase(0);
  }

  void reverse(){
    impl::node<T> * root_ptr = root_.switch_to_mutable();
    root_ptr->should_be_reversed = true;
  }

  const T& operator [] (size_t pos) const{
    assert(pos < this->size());
    return impl::search_in_node_subtree<T>(root_, pos)->value;
  }

  reference operator [] (size_t pos) {
    assert(pos < root_->size);
    return reference(this, pos);
  }

  const T& front(){
    return this->operator[](0);
  }

  const T& back(){
    return this->operator[](this->size() - 1);
  }

  size_t size() {
    return impl::size_of_node<T>(root_.get());
  }

  bool empty() {
    return (root_.get() == NULL);
  }

  void clear(){
    root_.switch_to_mutable();
    root_ = pds_ptr<impl::node<T> >();
  }

protected:

  pds_ptr<impl::node<T> > root_;

};

} //namespace pds

#endif /* PRESISTENT_ROPE_H__ */
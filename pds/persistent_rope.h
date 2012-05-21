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

namespace impl { // implementation

template <class T>
struct rope_node {

  explicit rope_node (T new_value = T())
    : value(new_value), size(1), left(), right(), should_be_reversed() {
    priority = std::rand();
    if (RAND_MAX == 32767) {
      priority += (std::rand() << 16);
    }
  }

  ~rope_node() {}

  unsigned int priority;
  T value;
  pds_ptr<rope_node<T> > left; // left child
  pds_ptr<rope_node<T> > right; // right child
  size_t size; // size of tree where [rope_node] is a root
  bool should_be_reversed;
};

template <class T>
size_t size_of_node(const rope_node<T> * node) {
  return node ? node->size : 0;
}

template <class T>
void update_size(rope_node<T> * node) {
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
pds_ptr<rope_node<T> > check_reversing(pds_ptr<rope_node<T> > current_node) {
  if (!current_node.get() || !current_node->should_be_reversed) {
    return current_node;
  }
  rope_node<T> * result_ptr = current_node.switch_to_mutable();

  std::swap(result_ptr->left, result_ptr->right);
  rope_node<T> * left_ptr = result_ptr->left.switch_to_mutable();
  rope_node<T> * right_ptr = result_ptr->right.switch_to_mutable();
  
  if (left_ptr) {
    left_ptr->should_be_reversed = !left_ptr->should_be_reversed;
  }
  if (right_ptr) {
    right_ptr->should_be_reversed = !right_ptr->should_be_reversed;
  }
  result_ptr->should_be_reversed = false;
  return current_node;
}

//search in subtree of [node] the element on position [pos]
template<class T>
pds_ptr<rope_node<T> > search_in_node_subtree(pds_ptr<rope_node<T> > current_node, size_t pos) {
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
pds_ptr<rope_node<T> > merge_nodes (pds_ptr<rope_node<T> > left, pds_ptr<rope_node<T> > right) {
  left = check_reversing<T>(left);
  right = check_reversing<T>(right);
  if (!left.get()) {
    return right;
  }
  if (!right.get()) {
    return left;
  }

  pds_ptr<rope_node<T> > result;
  if (rand_less(left->priority, right->priority)) {
    result = right;
    rope_node<T> * result_ptr = result.switch_to_mutable();
    result_ptr->left = merge_nodes(left, right->left);
    update_size<T>(result_ptr);
  } else {
    result = left;
    rope_node<T> * result_ptr = result.switch_to_mutable();
    result_ptr->right = merge_nodes(left->right, right);
    update_size<T>(result_ptr);
  }
  return result;
}

// it splits the subtree of [current_node] by [pos]
// result_1 contains first ([pos] - 1) elements of [current_node]
// result_2 contains remain elements of [current_rope]
template<class T>
std::pair<pds_ptr<rope_node<T> >, pds_ptr<rope_node<T> > > split_node (pds_ptr<rope_node<T> > current_node, size_t pos) {
  current_node = check_reversing<T>(current_node);
  if (!current_node.get()) {
    return std::make_pair(pds_ptr<rope_node<T> >(), pds_ptr<rope_node<T> >());
  }

  size_t current_pos = size_of_node<T>(current_node->left.get());

  pds_ptr<rope_node<T> > left;
  pds_ptr<rope_node<T> > right;
  pds_ptr<rope_node<T> > split;
  if (current_pos < pos) {
    std::tie(split, right) = split_node<T>(current_node->right, pos - current_pos - 1);
    left = current_node;
    rope_node<T> * left_ptr = left.switch_to_mutable();
    left_ptr->right = split;
    update_size<T>(left_ptr);
  } else {
    std::tie(left, split) = split_node<T>(current_node->left, pos);
    right = current_node;
    rope_node<T> * right_ptr = right.switch_to_mutable();
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

} // namespace impl

// preface: rand() is used here
template<class T>
class persistent_rope {
public:
  persistent_rope() : root_() {}
  
  explicit persistent_rope(const T &value) : root_(new impl::rope_node<T>(value)) {}

  class reference {
  public:

    operator T() const {
      return (*static_cast<const persistent_rope *>(owner_))[index_];
    }

    reference & operator = (const T& new_value) {
      owner_->erase(index_);
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
    pds_ptr<impl::rope_node<T> > left;
    pds_ptr<impl::rope_node<T> > right;
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

  void push_back(T value = T()) {
    this->push_back(persistent_rope<T>(value));
  }

  //delete elements with positions in [[pos_begin], [pos_end])
  bool erase(size_t pos_begin, size_t pos_end){
    if (pos_begin >= pos_end || pos_end > this->size()) {
      return false;
    }
    pds_ptr<impl::rope_node<T> > left;
    pds_ptr<impl::rope_node<T> > medium;
    pds_ptr<impl::rope_node<T> > right;

    std::tie(left, right) = impl::split_node<T>(root_, pos_begin);
    std::tie(medium, right) = impl::split_node<T>(right, pos_end - pos_begin);
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

  void reverse() {
    impl::rope_node<T> * root_ptr = root_.switch_to_mutable();
    root_ptr->should_be_reversed = !root_ptr->should_be_reversed;
  }

  const T operator [] (size_t pos) const {
    assert(pos < root_->size);
    return impl::search_in_node_subtree<T>(root_, pos)->value;
  }

  reference operator [] (size_t pos) {
    assert(pos < root_->size);
    return reference(this, pos);
  }

  const T front() const {
    return this->operator[](0);
  }

  const T back() const {
    return this->operator[](this->size() - 1);
  }

  size_t size() const {
    return impl::size_of_node<T>(root_.get());
  }

  bool empty() {
    return (root_.get() == NULL);
  }

  void clear() {
    root_.switch_to_mutable();
    root_ = pds_ptr<impl::rope_node<T> >();
  }

protected:

  pds_ptr<impl::rope_node<T> > root_;

};

} //namespace pds

#endif /* PRESISTENT_ROPE_H__ */

#ifndef PERSISTENT_HEAP_H_
#define PERSISTENT_HEAP_H_

#include <algorithm>
#include <functional>
#include <memory>
#include <iostream>

#include "pds_ptr.h"

namespace pds {

template<class T>
struct node {

  public:

  typedef node<T>* node_ptr;
  typedef pds_ptr<node> const_node_ptr;

  explicit node(const T &the_value) :
      value(the_value) {
    //std::cout << "new node value =" << value << std::endl;
  }

  ~node() {
    //std::cout << "delete node value =" << value << std::endl;
  }

  T value;
  const_node_ptr child[2];

};

template<class T, class Comparator = std::less<T> >
class persistent_heap {

  public:

  typedef typename node<T>::node_ptr node_ptr;
  typedef typename node<T>::const_node_ptr const_node_ptr;

  persistent_heap() :
      root_(), size_(0), top_size_(0) {
  }

  explicit persistent_heap(Comparator cmp) :
      cmp_(cmp), root_(), size_(0), top_size_(0) {
  }

  template<class Iterator>
  persistent_heap(Iterator begin, Iterator end) :
      root_(), size_(0), top_size_(0) {
    for (Iterator it = begin; it != end; ++it) {
      this->push(*it);
    }
  }

  template<class Iterator>
  persistent_heap(Iterator begin, Iterator end, Comparator cmp) :
      cmp_(cmp), root_(), size_(0), top_size_(0) {
    for (Iterator it = begin; it != end; ++it) {
      this->push(*it);
    }
  }

  template<class Iterator>
  void push(Iterator begin, Iterator end) {
    for (Iterator it = begin; it != end; ++it) {
      this->push(*it);
    }
  }

  void push(T value) {
    if (root_ == NULL) {
      construct_root(value);
    } else {
      node_ptr new_root = root_.switch_to_mutable();
      router r(new_root, size_, top_size_);
      while (!r.is_end()) {
        swap_if_less(value, r.current_value());
        r.down();
      }
      swap_if_less(value, r.current_value());
      r.next() = new node<T>(value);
      increment_size();
    }
  }

  void pop() {
    assert(!empty());
    if (size_ == 1) {
      delete_root();
    } else {
      decrement_size();
      node_ptr new_root = root_.switch_to_mutable();
      T value(delete_last_node(new_root));
      node_ptr& curr(new_root);
      while (curr->child[1] != NULL) {
        bool min_child_number = !(cmp_(curr->child[0]->value,
                                       curr->child[1]->value));
        T min_value(curr->child[min_child_number]->value);
        if (cmp_(min_value, value)) {
          curr->value = std::move(min_value);
          curr = curr->child[min_child_number].switch_to_mutable();
        } else {
          break; // goto next if
        }
      }
      bool only_left_child = (curr->child[1] == NULL &&
          curr->child[0] != NULL);
      if ((only_left_child && cmp_(curr->child[0]->value, value))) {
        curr->value = curr->child[0]->value;
        curr->child[0].switch_to_mutable()->value = std::move(value);
      } else {
        curr->value = std::move(value);
      }
    }
  }

  bool empty() const {
    return (root_ == NULL);
  }

  size_t size() const {
    return size_;
  }

  const T& top() const {
    return root_->value;
  }

  private:

  Comparator cmp_;
  const_node_ptr root_;

  size_t size_;

  // size without last level (if size_ is (2^n -1)  => size_ == top_size_)
  size_t top_size_;

  void construct_root(const T& value) {
    root_ = new node<T>(value);
    size_ = 1;
    top_size_ = 1;
  }

  void delete_root() {
    root_.reset();
    size_ = 0;
    top_size_ = 0;
  }

  void swap_if_less(T& a, T& b) {
    if (cmp_(a, b)) {
      std::swap(a, b);
    }
  }

  void increment_size() {
    size_++;
    if (size_ - 1 == 2 * top_size_) {
      top_size_ = size_;
    }
  }

  void decrement_size() {
    if (size_ == top_size_) {
      top_size_ >>= 1;
    }
    size_--;
  }

  T delete_last_node(const node_ptr& new_root) {
    router r(new_root, size_, top_size_);
    while (!r.is_end()) {
      r.down();
    }
    T value(r.next()->value);
    r.next().reset();
    return value;
  }

  /*
   * this class help us to come in last node
   */

  class router {
    public:
    router(const node_ptr &base_ptr, size_t size, size_t top_size) :
        current_(base_ptr),
        current_top_size_(top_size),
        current_last_level_(size - top_size) {
      count_next();
    }

    // move current node to next on the way

    void down() {
      current_ = next().switch_to_mutable();
      current_top_size_ >>= 1;
      current_last_level_ %= current_top_size_ + 1;
      count_next();
    }

    // return const_node_ptr to next node on the way

    const_node_ptr& next() {
      return current_->child[next_];
    }

    // return true when next is last node

    bool is_end() {
      return ((current_top_size_ == 1
          && (current_last_level_ == 0 || current_last_level_ == 1)));
    }

    T& current_value() {
      return current_->value;
    }

    private:

    node_ptr current_;

    void count_next() {
      next_ = ((current_top_size_ + 1) / 2 <= current_last_level_);
    }

    bool next_;
    size_t current_top_size_;
    size_t current_last_level_;
  };

};

} // namespace pds

#endif /* PERSISTENT_HEAP_H_ */

#ifndef PERSISTENT_HEAP_H_
#define PERSISTENT_HEAP_H_

#include <algorithm>
#include <functional>
#include <memory>
#include <iostream>

namespace pds {



template<class T>
struct node {


  typedef node<T>* node_ptr;

  class const_node_ptr : public std::shared_ptr<const node<T> > {

   public:

    node_ptr clone() {
      node_ptr my_clone(new node(**this));
      this->base_class::operator=(static_cast<base_class>(my_clone));
      return my_clone;
    }

    void make_leaf(T value) {
      this->reset(new node(value));
    }

   private:

    typedef std::shared_ptr<const node<T> > base_class;

  };


  explicit node(const T &the_value) :
      value(the_value) {
    //std::cout << "new node value =" << value_ << std::endl;
  }

  explicit node(const_node_ptr base_node) :
      value(base_node->value) {
    child[0] = base_node->child[0];
    child[1] = base_node->child[1];
    //std::cout << "new node value =" << value_ << std::endl;
  }

  ~node() {
    //std::cout << "delete node value =" << value << std::endl;
  }

  T value;

  const_node_ptr child[2];

 //private:

};



template<class T, class Comparator = std::less<T> >
class persistent_heap {

 public:

  typedef typename node<T>::node_ptr node_ptr;
  typedef typename node<T>::const_node_ptr const_node_ptr;

  persistent_heap() :
      root_(), size_(0), top_size_(0) {
  }

  persistent_heap(const persistent_heap<T, Comparator> &base_heap) :
      root_(base_heap.root_),
      size_(base_heap.size_),
      top_size_(base_heap.top_size_) {
  }

  void operator =(const persistent_heap<T, Comparator> &base_heap) {
    root_ = base_heap.root_;
    size_ = base_heap.size_;
    top_size_ = base_heap.top_size_;
  }

  void push(T value) {
    if (root_ == NULL) {
      root_.make_leaf(value);
      size_ = 1;
      top_size_ = 1;
    } else {
      node_ptr new_root = root_.clone();
      router r(new_root, size_, top_size_);
      while (!r.end_of_routing()) {;
        swap_if_less(value, r.current_value());
        r.down();
      }
      swap_if_less(value, r.current_value());
      r.next().make_leaf(value);
      increment_size();
    }
  }

  T pop() {

    assert(!empty());
    T result = minimum();
    if (size_ == 1) {
      root_.reset();
      size_ = 0;
      top_size_ = 0;
    } else {
      decrement_size();
      node_ptr new_root = root_.clone();
      T value = delete_last_node(new_root);
      node_ptr& curr(new_root);
      while (curr->child[1] != NULL) {
        bool min_child_no;
        min_child_no = !(cmp_(curr->child[0]->value,
                           curr->child[1]->value));
        T min_value = curr->child[min_child_no]->value;
        if (cmp_(min_value, value)) {
          curr->value = min_value;
          curr = curr->child[min_child_no].clone();
        } else {
          break; // goto next if
        }
      }
      if ((curr->child[1] == NULL && curr->child[0] != NULL
          && cmp_(curr->child[0]->value, value))) {
        curr->value = curr->child[0]->value;
        curr->child[0].clone()->value = value;
      } else {
        curr->value = value;
      }
    }
    return result;
  }

  bool empty() const {
    return (root_ == NULL);
  }

  size_t size() const {
    return size_;
  }

  T minimum() const {
    return (root_)->value;
  }

 private:

  Comparator cmp_;
  const_node_ptr root_;

  size_t size_;

  // size without last level (if size_ is (2^n -1)  => size_ == top_size_)
  size_t top_size_;

  void swap_if_less(T& a, T& b) {
    if (cmp_(a, b)) {
      std::swap(a, b);
    }
  }

  void increment_size(){
    size_++;
    if (size_ - 1 == 2 * top_size_) {
      top_size_ = size_;
    }
  }

  void decrement_size(){
    if (size_ == top_size_) {
      top_size_ >>= 1;
    }
    size_--;
  }

  T delete_last_node(const node_ptr& new_root){
    router r(new_root, size_, top_size_);
    while (!r.end_of_routing()) {
      r.down();
    }
    T value = r.next()->value;
    r.next().reset();
    return value;
  }

  class router {
    public:
    router(const node_ptr &base_ptr, size_t size, size_t top_size) :
        current_(base_ptr),
        current_top_size_(top_size),
        current_last_level_(size - top_size) {
      count_next();
    }

    void down() {
      current_ = next().clone();
      current_top_size_ >>= 1;
      current_last_level_ %= current_top_size_ + 1;
      count_next();
    }

    const_node_ptr& next() {
      return current_->child[next_];
    }

    bool end_of_routing() {
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

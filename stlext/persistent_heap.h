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

    void operator =(const node_ptr &base_node){
      this->base_class::operator=(base_node);
    }


    node_ptr clone(){
      node_ptr my_clone(new node(**this));
      this->base_class::operator=(static_cast<base_class>(my_clone));
      return my_clone;
    }

    void make_leaf(T value){
      this->reset(new node(value));
    }

   private:

    typedef std::shared_ptr<const node<T> > base_class;

  };






  ~node() {
    //std::cout << "delete node value =" << value_ << std::endl;
  }

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

  T value;

  const_node_ptr child[2];

 private:

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
      node_ptr base = root_.clone();
      router r(base, size_, top_size_);
      while (!r.end_of_routing()) {;
        swap_if_less(value, r.current_->value);
        r.down();
      }
      swap_if_less(value, r.current_->value);
      r.current_->child[r.next()].make_leaf(value);

      size_++;
      if (size_ - 1 == 2 * top_size_) {
        top_size_ = size_;
      }
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
      if (size_ == top_size_) {
        top_size_ >>= 1;
      }
      size_--;
      node_ptr base = root_.clone();
      router r(base, size_, top_size_);
      node_ptr new_root = r.current_;
      while (!r.end_of_routing()) {
        r.down();
      }
      T value = r.current_->child[r.next()]->value;
      r.current_->child[r.next()].reset();
      node_ptr* double_ptr(&new_root);
      node_ptr temp;
      while ((*double_ptr)->child[1] != NULL) {
        bool min_child;
        T min_child_value;
        min_child = !(cmp_((*double_ptr)->child[0]->value,
                           (*double_ptr)->child[1]->value));
        min_child_value = (*double_ptr)->child[min_child]->value;
        if (cmp_(min_child_value, value)) {
          (*double_ptr)->value = min_child_value;
          temp = (*double_ptr)->child[min_child].clone();
          double_ptr = &temp;
        } else {
          break;
        }
      }
      if (((*double_ptr)->child[1] == NULL && (*double_ptr)->child[0] != NULL
          && cmp_((*double_ptr)->child[0]->value, value))) {

        (*double_ptr)->value = (*double_ptr)->child[0]->value;

        temp = (*double_ptr)->child[0].clone();
        temp->value = value;

      } else {
        (*double_ptr)->value = value;
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

 protected:

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

  class router {
    public:
    router(node_ptr base_ptr, size_t size, size_t top_size) :
        current_(base_ptr),
        current_top_size_(top_size),
        current_last_level_(size - top_size) {
      count_next();
    }

    void down() {
      //node_ptr next_node(new node<T>(current_->child_[next_]));
      //current_->child_[next_] = next_node;
      //current_ = next_node;
      current_ = current_->child[next_].clone();
      current_top_size_ >>= 1;
      current_last_level_ %= current_top_size_ + 1;
      count_next();
    }

    bool next() {
      return next_;
    }

    bool end_of_routing() {
      return ((current_top_size_ == 1
          && (current_last_level_ == 0 || current_last_level_ == 1)));
    }

    node_ptr current_;

   private:

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

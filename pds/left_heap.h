#ifndef PERSISTENT_LEFT_HEAP_H_
#define PERSISTENT_LEFT_HEAP_H_

#include <algorithm>
#include <functional>
#include <vector>
#include <memory>
#include <iostream>


/*
  TODO:
  tests plus struct test (friend function)
  time tests
*/

namespace psd{

namespace internal{
template<class T>
struct node{
public:
  typedef node<T>* node_ptr;

  class const_node_ptr : public std::shared_ptr<const node<T>> {
  public:
    const_node_ptr operator=(node_ptr nd){
      this->base_class::operator=(static_cast<base_class>(nd));
      return *this;
    }
    node_ptr clone(){
      node_ptr my_clone(new node(**this));
      this->base_class::operator=(static_cast<base_class>(my_clone));
      return my_clone;
    }
    void make_leaf(const T &value) {
      this->reset(new node(value));
    }

  private:
    typedef std::shared_ptr<const node<T>> base_class;
  };

  explicit node (const T &key)
    : key(key), s_value(1) {
  }

  node (const T &key, const_node_ptr left, const_node_ptr right)
    : key(key) {
    left_child=left;
    right_child=right;
    count_s_value();
  }

  void fix_leftlist(){
    if(left_child == nullptr) {
      std::swap(left_child,right_child);
      s_value = 1;
    }
    else if(right_child == nullptr){
      s_value = 1;
    }else{
      if(left_child->s_value < right_child->s_value){
        std::swap(left_child,right_child);
      }
      s_value = right_child->s_value + 1;
    }
  }

  T key;

  const_node_ptr left_child;
  const_node_ptr right_child;

  size_t s_value;
  /*
    The s-value of a node is the distance from
  that node to the nearest leaf of the extended
  binary representation of the tree
  http://en.wikipedia.org/wiki/Leftist_tree
  */
};

template<class T, class Comparator>
typename node<T>::const_node_ptr merge_nodes(
      typename node<T>::const_node_ptr left, 
      typename node<T>::const_node_ptr right,
      Comparator cmp){
  if(left==nullptr)
    return right;
  if(right==nullptr)
    return left;
  if(cmp(right->key, left->key)) {  
    std::swap(left, right);
  }
  typename node<T>::node_ptr mutable_left = left.clone();
                    // left point to new "clonned" version of node

  mutable_left->right_child = merge_nodes<T, Comparator> 
                        (mutable_left->right_child, right, cmp);

  mutable_left->fix_leftlist();

  return left;
}

template<class T, class Comparator>
struct node_cmp{
public:
  typedef std::unique_ptr<node<T>> unique_node;
  node_cmp(Comparator cmp) : cmp_(cmp) {
  }
  bool operator()(const unique_node &left, const unique_node &right) {
    return cmp_(right->key,left->key);
  }
private:
  Comparator cmp_;
};

} // namespace internal 

template<class T, class Comparator = std::less<T> >
class left_heap{
public:
  typedef typename internal::node<T>::node_ptr node_ptr;
  typedef typename internal::node<T>::const_node_ptr const_node_ptr;
  
  left_heap()
    : root_(), size_(0) {
  }
    
  explicit left_heap(Comparator cmp)
    : root_(), size_(0), cmp_(cmp) {
  }

  template<class Iterator>
  left_heap(Iterator begin, Iterator end) :
      root_(), size_(0) {
    clear();
    push(begin,end);
  }


  template<class Iterator>
  left_heap(Iterator begin, Iterator end, Comparator cmp) :
    root_(), size_(0), cmp_(cmp) {
    clear();
    push(begin, end);
  }

  left_heap(const left_heap<T> &base_heap)
    : root_(base_heap.root_), size_(base_heap.size_), cmp_(base_heap.cmp_) {
  }

  void operator = (const left_heap<T> &base_heap) {
    root_ = base_heap.root_;  
    size_ = base_heap.size_;
    cmp_  = base_heap.cmp_ ;
  }
  
  ~left_heap(){
    clear();
  }
  
  void push(const T &key){
    if(root_ == nullptr){
      size_ = 1;
      root_.make_leaf(key);
    }
    else{
      size_++;
      const_node_ptr new_node;
      new_node.make_leaf(key);
      root_ = internal::merge_nodes<T, Comparator> (root_, new_node, cmp_);
    }
  }

  template<class Iterator>
  void push(Iterator begin, Iterator end) {
    typedef std::unique_ptr<internal::node<T>> unique_node;
    std::vector<unique_node> new_elements;
    for (Iterator it = begin; it != end; ++it) {
      new_elements.push_back(unique_node(new internal::node<T>(*it)));
    }
    std::make_heap(new_elements.begin(),new_elements.end(),internal::node_cmp<T,Comparator>(cmp_));
    size_t n = new_elements.size();
    for(size_t i = n-1; i > 0; i--) {
      new_elements[i]->fix_leftlist();
      size_t parent_id = (i-1)/2;
      if( i%2 == 0 ) { 
        new_elements[parent_id]->right_child = new_elements[i].release();
      }
      else{
        new_elements[parent_id]->left_child  = new_elements[i].release();
      }
    }
    new_elements[0]->fix_leftlist();
    const_node_ptr new_root;
    new_root = new_elements[0].release();
    root_ = internal::merge_nodes<T> (root_, new_root, cmp_);
  }

  void pop(){
    root_ = internal::merge_nodes<T, Comparator> 
              (root_->left_child, root_->right_child, cmp_);
    size_--;
  }

  void merge(const left_heap &tree){
    root_ = internal::merge_nodes<T, Comparator> (root_, tree.root_, cmp_);
    size_ += tree.size();
  }

  T top() const{
    return root_->key;
  }

  size_t size() const{
    return size_;
  }

  void clear(){
    while(root_!=nullptr){
      root_ = root_->left_child; // We save shared_ptr to left child, and
                                // "delete" old root and right child.
                                // Height of right subtree less O(log N)
    }
    size_ = 0;
  }

  bool empty() const{
    return size_==0;
  }

private:
  const_node_ptr root_;
  size_t size_;
  Comparator cmp_;
};

} //namespace psd

#endif
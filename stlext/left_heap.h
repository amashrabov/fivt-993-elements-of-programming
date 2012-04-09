#ifndef PERSISTENT_LEFT_HEAP_H_
#define PERSISTENT_LEFT_HEAP_H_

#include <algorithm>
#include <functional>
#include <memory>
#include <iostream>


/*
	TODO:
	tests plus struct test (friend function)
	time tests
*/

namespace psd{

template<class T>
struct node{
public:
  typedef node<T>* node_ptr;
  
  class const_node_ptr : public std::shared_ptr<const node<T>> {
	public:
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
    : key(key),s_value(1){
  }

  node (const T &key, const_node_ptr left, const_node_ptr right)
    : key(key){
    left_child=left;
    right_child=right;
    count_s_value();
  }

  void fix_leftlist(){
    if(left_child == nullptr){
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
			typename node<T>::const_node_ptr Left, 
			typename node<T>::const_node_ptr Right,
			Comparator &cmp){
  if(Left==nullptr)
    return Right;
  if(Right==nullptr)
    return Left;
  if(cmp(Right->key, Left->key)) {  
		std::swap(Left,Right);
  }
  typename node<T>::node_ptr left = Left.clone();

  left->right_child = merge_nodes<T, Comparator> 
												(left->right_child, Right, cmp);
  left->fix_leftlist();

  return Left;
}

template<class T, class Comparator = std::less<T> >
class left_heap{
public:
  typedef typename node<T>::node_ptr node_ptr;
  typedef typename node<T>::const_node_ptr const_node_ptr;

  left_heap()
    : root_(),size_(0) {
  }

  left_heap(Comparator cmp)
    : root_(),size_(0), cmp_(cmp) {
  }

	template<class Iterator>
  left_heap(Iterator begin, Iterator end) :
      root_(), size_(0) {
    for (Iterator it = begin; it != end; ++it) {
      this->push(*it);
    }
  }


  template<class Iterator>
  left_heap(Iterator begin, Iterator end, Comparator cmp) :
      cmp_(cmp), root_(), size_(0) {
    for (Iterator it = begin; it != end; ++it) {
      this->push(*it);
    }
  }

  left_heap(const left_heap<T> &base_heap)
    : root_(base_heap.root_), size_(base_heap.size_), cmp_(base_heap.cmp_) {
  }

  void operator = (const left_heap<T> &base_heap) {
    root_ = base_heap.root_;  
    size_ = base_heap.size_;
		cmp_  = base_heap.cmp_ ;
  }

/*
	~left_heap(){
		while(size_ >= max_destructor_size_){
			pop();
		}	
	}
	*/
	
	~left_heap(){
		while(root_!=nullptr){
			root_ = root_->left_child; // We save shared_ptr to left child, and
																// "delete" old root and right child.
																// Heigt of right subtree less O(log N)
		}
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
      root_ = merge_nodes<T, Comparator> (root_, new_node, cmp_);
    }
  }
  void pop(){
    root_ = merge_nodes<T, Comparator> 
							(root_->left_child, root_->right_child, cmp_);
    size_--;
  }
  void merge(const left_heap &tree){
    root_ = merge_nodes<T, Comparator> (root_, tree.root_, cmp_);
    size_ += tree.size();
  }
  T top() const{
    return root_->key;
  }
  size_t size() const{
    return size_;
  }
  bool empty() const{
    return size_==0;
  }
private:
  const_node_ptr root_;
  size_t size_;
	Comparator cmp_;
	/*
#ifdef _DEBUG
	static const size_t max_destructor_size_ = 500; // ????
#else 
	static const size_t max_destructor_size_ = 10000; // ????
#endif //ifdef DEBUG
	*/
};

} //namespace psd

#endif
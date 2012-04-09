#ifndef PERSISTENT_LEFT_HEAP_H
#define PERSISTENT_LEFT_HEAP_H_

#include <algorithm>
#include <functional>
#include <memory>
#include <iostream>

namespace stlext{

template<class T >
struct node{
public:
	typedef node<T >* node_ptr;
	
  class const_node_ptr : public std::shared_ptr<const node<T > > {
   public:
    node_ptr clone(){
      node_ptr my_clone(new node(**this));
      this->base_class::operator=(static_cast<base_class>(my_clone));
      return my_clone;
    }
    void make_leaf(T value) {
      this->reset(new node(value));
    }

   private:

    typedef std::shared_ptr<const node<T > > base_class;

  };

	explicit node (T key)
		: key(key),s_value(1){
	}

	node (T key, const_node_ptr left, const_node_ptr right)
		: key(key){
		left_child=left;
		right_child=right;
		count_s_value();
	}
	void count_s_value(){
		if(left_child==NULL){
			left_child=right_child;
			right_child=const_node_ptr();
			s_value=1;
		}
		else{
			if(left_child->s_value < right_child->s_value){
				typename node<T >::const_node_ptr tmp = left_child;
				left_child = right_child;
				right_child = tmp;
			}
			s_value = right_child->s_value + 1;
		}
	}

	T key;
	size_t s_value;
	const_node_ptr right_child, left_child;

};

template<class T >
typename node<T >::const_node_ptr merge_nodes(typename node<T >::const_node_ptr Left, typename node<T >::const_node_ptr Right){
	if(Left==NULL)
		return Right;
	if(Right==NULL)
		return Left;
	if(Left->key > Right->key) {  
		typename node<T >::const_node_ptr tmp = Left;
		Left = Right;
		Right = tmp;
  }
	typename node<T >::node_ptr left = Left.clone();

  left->right_child = merge_nodes<T >(left->right_child, Right);
	left->count_s_value();

	return Left;
}

template<class T >
class left_heap{
public:
  typedef typename node<T >::node_ptr node_ptr;
  typedef typename node<T >::const_node_ptr const_node_ptr;

	left_heap()
		: root_(),size_(0) {
	}
  left_heap(const left_heap<T > &base_heap)
    : root_(base_heap.root_), size_(base_heap.size_) {
  }

  void operator = (const left_heap<T > &base_heap) {
    root_ = base_heap.root_;	
    size_ = base_heap.size_;
  }
	void push(T key){
		if(root_ == NULL){
			size_ = 1;
			root_.make_leaf(key);
		}
		else{
			size_++;
			const_node_ptr new_node;
			new_node.make_leaf(key);
			root_ = merge_nodes<T > (root_, new_node);
		}
	}
	T pop(){
		T result = top();
		root_ = merge_nodes<T >(root_->left_child, root_->right_child);
		size_--;
		return result;
	}
	void merge(const left_heap &tree){
		root_ = merge_nodes<T >(root_, tree.root_);
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
};

} //namespace stlext

#endif
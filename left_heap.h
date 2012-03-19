#ifndef PERSISTENT_LEFT_HEAP_H
#define PERSISTENT_LEFT_HEAP_H_

#include <algorithm>
#include <functional>
#include <memory>
#include <iostream>

namespace stlext{

template<class T>
struct Node{
public:
	Node(T Key)
		: key(Key),s_value(1){
	}
	T key;
	int s_value;
	Node *rightChild, *leftChild;
private:
};

template<class T>
class left_heap{
public:
	left_heap()
		: root_(NULL),size_(0){
	}
private:
	Node<T> *root_;
	size_t size_;
	
};

} //namespace stlext

#endif
#ifndef PERSISTENT_STACK
#define PERSISTENT_STACK

#include <memory>
#include <assert.h>

namespace pds{

template<class T>
class node{

	typedef std::shared_ptr<const node<T>> const_node_ptr;

	const_node_ptr prev; 
	T value;

public:

	node(const T& val, const_node_ptr p): 
	  value(val), prev(p) {};

	const T& get_value() const{
		return value;
	}

	const_node_ptr get_previous() const{
		return prev;
	}

}; // class node

template<class T>
class persistent_stack{
	
	typedef std::shared_ptr<const node<T>> const_node_ptr;
	
	const_node_ptr _top;
	size_t _size;

public:

	persistent_stack():
		_top(0), _size(0){}

	void push(const T& value){
		_size++;
		const_node_ptr new_top(new node<T>(value, _top));
		_top = new_top;
	}

	const T& top() const{
		assert(_size != 0);
		const T& result = _top->get_value();
		return result;
	}

	void pop(){
		assert(_size != 0);
		_size--;
		_top = _top->get_previous();
	}

	bool empty() const{
		return (_size == 0);
	}

	size_t size() const{
		return _size;
	}

}; // class persistent_stack

} // namespace pds

#endif // PERSISTENT_STACK

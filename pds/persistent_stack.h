#ifndef PERSISTENT_STACK
#define PERSISTENT_STACK

#include <memory>

namespace psd{

class empty{};

template<class T>
class node{

	typedef std::shared_ptr<const node<T>> const_node_ptr;

	const_node_ptr prev; 
	T value;

public:

	node(T val, const_node_ptr p): 
	  value(val), prev(p) {};

	T get_value() const{
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

	void push(T value){
		_size++;
		const_node_ptr new_top(new node<T>(value, _top));
		_top = new_top;
	}

	T top(){
		if (_size == 0) throw empty();
		else {
			T result = _top->get_value();
			return result;
		}
	}

	void pop(){
		if (_size == 0) throw psd::empty();
		else {
			_size--;
			_top = _top->get_previous();
		}
	}

	bool empty(){
		return (_size == 0);
	}

	size_t size(){
		return _size;
	}

}; // class persistent_stack

} // namespace psd

#endif // PERSISTENT_STACK
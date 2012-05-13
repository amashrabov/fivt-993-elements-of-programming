#ifndef __PRESISTENT_ROPE_H__
#define __PRESISTENT_ROPE_H__

#include <assert.h>
#include <climits>
#include <functional>
#include <memory>

namespace pds{ // Persistent Data Structures

template <class T>
struct node{
	typedef node<T> * node_ptr;

	class const_node_ptr : public std::shared_ptr<const node<T> > {
	public:
		const_node_ptr() : base_class(){}

		const_node_ptr(node_ptr node) : base_class(node){}

		const_node_ptr operator = (const_node_ptr node){
			this->base_class::operator = (static_cast <base_class>(node));
			return *this;
		}
	private:
		typedef std::shared_ptr<const node<T> > base_class;
	};

	explicit node (T new_value = T()) : value(new_value), size(1), child(){
		priority = std::rand() % INT_MAX + 1;
	}

	~node(){}

	void update_size(){
		size = 1;
		for (int i = 0; i < 2; i++){
			if (child[i] != NULL) size += child[i]->size;
		}
	}

	void copy_info(const_node_ptr node, bool copy_child_0, bool copy_child_1){
		this->value = node->value;
		this->priority = node->priority;
		if (copy_child_0) this->child[0] = node->child[0];
		if (copy_child_1) this->child[1] = node->child[1];
		this->update_size();
	}

	int priority;
	T value;
	const_node_ptr child[2];
	size_t size; // size of tree where [node] is a root
};

template<class T>
typename node<T>::const_node_ptr merge_nodes
	(typename node<T>::const_node_ptr node_1,
	typename node<T>::const_node_ptr node_2){
	if (node_1 == NULL) return node_2;
	if (node_2 == NULL) return node_1;
	node<T>::node_ptr result = new node<T>();
	if (rand_less(node_1->priority, node_2->priority)){
		result->copy_info(node_2, false, true);
		result->child[0] = merge_nodes<T>(node_1, node_2->child[0]);
	}else{
		result->copy_info(node_1, true, false);
		result->child[1] = merge_nodes<T>(node_1->child[1], node_2);
	}
	result->update_size();
	return node<T>::const_node_ptr(result);
}

// it splits subtree of [current_node] by [pos]
// result_1 = elements of [current_node] with positions in [0, pos)
// result_2 = elements of [current_node] with positions in [pos, [current_node]->size)
template<class T>
void split_node(typename node<T>::const_node_ptr current_node,
	size_t pos,
	typename node<T>::const_node_ptr * result_1,
	typename node<T>::const_node_ptr * result_2){
		if (current_node == NULL){
			*result_1 = NULL;
			*result_2 = NULL;
			return;
		}
		size_t current_pos = 0;
		if (current_node->child[0] != NULL) current_pos = current_node->child[0]->size;

		node<T>::const_node_ptr split = NULL;
		node<T>::node_ptr result = new node<T>();
		if (current_pos < pos){
			if (current_node->child[1] != NULL){
				split_node<T>(current_node->child[1], pos - current_pos - 1, &split, result_2);
			}else{
				*result_2 = NULL;
			}
			result->copy_info(current_node, true, false);
			result->child[1] = node<T>::const_node_ptr(split);
			result->update_size();
			*result_1 = node<T>::const_node_ptr(result);
		}else{
			if (current_node->child[0] == NULL){
				split_node<T>(current_node->child[0], pos, result_1, &split);
			}else{
				*result_1 = NULL;
			}
			result->copy_info(current_node, false, true);
			result->child[0] = node<T>::const_node_ptr(split);
			result->update_size();
			*result_2 = node<T>::const_node_ptr(result);
		}
}

//search in subtree of [node] the element on position [pos]
template<class T>
typename node<T>::const_node_ptr search_in_node_subtree(typename node<T>::const_node_ptr node, size_t pos){
	if (node == NULL) return NULL;
	size_t current_pos = 0;
	if (node->child[0] != NULL) current_pos += node->child[0]->size;
	if (current_pos < pos) return search_in_node_subtree<T>(node->child[1], pos - current_pos - 1);
	if (current_pos > pos) return search_in_node_subtree<T>(node->child[0], pos);
	return node;
}

template<class T>
typename node<T>::const_node_ptr reverse_node(typename node<T>::const_node_ptr base_node){
	if (base_node == NULL) return NULL;
	node<T>::node_ptr result = new node<T>();
	result->copy_info(base_node, false, false);
	result->child[0] = reverse_node<T>(base_node->child[1]);
	result->child[1] = reverse_node<T>(base_node->child[0]);
	result->size = base_node->size;
	return node<T>::const_node_ptr(result);
}

//preface: rand() is used here
template<class T>
class persistent_rope{
public:
	persistent_rope() : root_() {}

	persistent_rope(typename node<T>::const_node_ptr new_root) : root_(new_root) {}

	persistent_rope(const T root_value) : root_(new node<T>(root_value)) {}

	persistent_rope(const persistent_rope<T> &base_rope) : root_(base_rope->root_) {}

	void operator =(const persistent_rope<T> &base_rope){
		root_ = base_rope->root_;
	}

	bool push(size_t pos, T value){
		if (pos > this->size()) return false;
		typename node<T>::const_node_ptr left = NULL;
		typename node<T>::const_node_ptr right = NULL;
		split_node<T>(root_, pos, &left, &right);
		left = merge_nodes<T>(left, node<T>::const_node_ptr(new node<T>(value)));
		root_ = merge_nodes<T>(left, right);
		return true;
	}

	bool push_back(T value){
		return this->push(this->size(), value);
	}

	bool erase(size_t pos){
		if (pos >= this->size()) return false;
		typename node<T>::const_node_ptr left = NULL;
		typename node<T>::const_node_ptr right = NULL;
		split_node<T>(root_, pos, &left, &right);
		typename node<T>::const_node_ptr medium = NULL;
		split_node<T>(right, 1, &medium, &right);
		root_ = merge_nodes<T>(left, right);
		return true;
	}

	bool pop_back(){
		if (root_ == NULL) return true;
		return this->erase(this->size() - 1);
	}

	void reverse(){
		root_ = reverse_node<T>(root_);
	}

	T operator [] (size_t pos){
		assert(pos < this->size());
		return search_in_node_subtree<T>(root_, pos)->value;
	}

	size_t size(){
		if (root_ == NULL) return 0;
		return root_->size;
	}

	bool empty(){
		return (root_ == NULL);
	}

protected:
	typename node<T>::const_node_ptr root_;
};

bool rand_less(const int priority_1, const int priority_2){
	if (priority_1 == priority_2){
		return (std::rand() % 2);
	}
	return std::less<int>()(priority_1, priority_2);
}

} //namespace pds

#endif // __PRESISTENT_ROPE_H__
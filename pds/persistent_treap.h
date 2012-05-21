#pragma once
#include <vector>
#include <algorithm>
#include <cstdio>
#include <iostream>
#include "pds_ptr.h"


template<class T>
struct node{

	typedef node<T>* pnode;

	class const_spnode : public pds::pds_ptr<node<T>> {
	public:

		void make_leaf(const T & value) {
			pnode new_node = new node(value);
			this->reset(new_node);		
		}

		void make_virt_leaf(const T & value) {
			pnode new_node = new node(value);
			new_node->virt = true;
			this->reset(new_node);
		}

		void nullify(){
			pnode new_node = NULL;
			this->reset(new_node);
		}

	private:
		typedef std::shared_ptr<const node<T> > base_class;
	};


private:
	int get_random_int(){
		if (RAND_MAX > 32767) return rand();
		return (rand() << 15) ^ (rand());
	}
public:


	T value;
	int priority;
	int size;
	bool virt;
	const_spnode left;
	const_spnode right;

	explicit node (const T &value): value(value), 
		priority(get_random_int()), 
		size(1),
		virt(false){
			left.nullify();
			right.nullify();		
	}

};




template<class T, class Comparator = std::less<T> >
class persistent_treap{
public:
	typedef typename node<T>::pnode pnode;
	typedef typename node<T>::const_spnode const_spnode;

private:	
	const_spnode root_;
	Comparator cmp_;

public:
	persistent_treap() : root_(){		
	}

	persistent_treap (const persistent_treap<T, Comparator> & base_treap):
	root_(base_treap.root_),
		cmp_(base_treap.cmp_){
	}

	void operator =(const persistent_treap<T, Comparator> & base_treap) {
		this->root_ = base_treap.root_;		
		this->cmp_ = base_treap.cmp_;		
	}

	bool push(const T &value){
		if (exist(value)) return false;
		if (root_ == NULL) {			
			root_.make_leaf(value);
			return true;
		}
		pnode new_root = root_.switch_to_mutable(); 		
		const_spnode const_new_node_ptr;
		const_new_node_ptr.make_leaf(value);
		pnode new_node_ptr = const_new_node_ptr.switch_to_mutable();
		insert(new_root, new_node_ptr);		
		return true;
	}

	void clear(){
		root_.nullify();
	}

	bool pop(const T & value){
		if (!exist_(root_, value)) return false;
		pnode new_root = root_.switch_to_mutable();
		erase(new_root, value);
		if (new_root->virt) clear();
		return true;
	}	

	const bool exist(const T & value){
		return exist_(root_, value);
	}

	const T get_kth(int value){
		return get_kth_(root_, value);
	}

	const int size(){
		return (get_size(root_));		
	}

	const void print_tree(){
		print_node(root_);
	}

	const bool empty(){
		return (size() == 0 ? true : false);
	}

private:

	void print_node(const const_spnode & root){
		if (root == NULL) std::cout<< "null" << std::endl; else
		{
			std::cout << root->value << " " << root->priority << std::endl;
			print_node(root->left);
			print_node(root->right);
		}
	}

	void check_virt_node(const_spnode & nod){
		if (nod == NULL) {
			nod.make_virt_leaf(T());
		} else
			if (nod->virt) nod.nullify();
		return;		
	}

	void check_virt_children(pnode & nod){
		if (nod->left == NULL) nod->left.make_virt_leaf(T()); else
			if (nod->left->virt) nod->left.nullify();
		if (nod->right == NULL) nod->right.make_virt_leaf(T()); else
			if (nod->right->virt) nod->right.nullify();
		return;
	}

	void nullify_virt_children(pnode & nod){
		if (nod->left != NULL && nod->left->virt) nod->left.nullify();
		if (nod->right != NULL && nod->right->virt) nod->right.nullify();
	}

	void insert(pnode & root, pnode & new_node){
		if (root->virt) {	
			*root = *new_node;
		} else					
			if (compare_priority(root, new_node) ){	
				check_virt_children(new_node);				
				pnode new_left = new_node->left.switch_to_mutable();				
				pnode new_right = new_node->right.switch_to_mutable();				
				split (root, new_node->value, new_left, new_right);		
				check_virt_children(new_node);							
				*root = *new_node;				
			} else {				
				check_virt_children(root);				
				pnode new_left = root->left.switch_to_mutable();
				pnode new_right = root->right.switch_to_mutable();
				insert ( (cmp_(new_node->value, root->value) ? new_left : new_right), new_node);				
				check_virt_children(root);								
			}
			upd_size(root);
	}

	T get_kth_(const_spnode root, int index){
		int left_size = get_size(root->left);
		int right_size = get_size(root->right);
		if (left_size == index - 1) return root->value;
		if (left_size >= index) return get_kth_(root->left, index);
		return get_kth_(root->right, index - left_size - 1);
	}

	bool exist_(const_spnode root, const T & value){
		if (root == NULL) return false;
		if (root->value == value) return true;
		if (cmp_(value, root->value)) return exist_(root->left, value);
		else return exist_(root->right, value);
	}

	bool compare_priority(pnode n1, pnode n2){
		if (n1->priority < n2->priority ||
			(n1->priority == n2->priority && rand()%2 == 0) ) return true; 
		else return false;
	}

	void erase(pnode & root, const T & value){
		if (cmp_(value,root->value)){
			pnode new_root;
			new_root = root->left.switch_to_mutable();
			erase (new_root, value);
			nullify_virt_children(root);
		} else if (cmp_(root->value, value) ){
			pnode new_root;
			new_root = root->right.switch_to_mutable();
			erase (new_root, value);
			nullify_virt_children(root);
		} else {
			if (root->left == NULL) {
				if (root->right == NULL) root->virt = true; else
					*root = *root->right.switch_to_mutable();
			} else
				if (root->right == NULL){
					*root = *root->left.switch_to_mutable();
				}
				nullify_virt_children(root);
		}
		upd_size(root);
	}

	int get_size(pnode root){
		if (root == NULL) return 0; else return root->size;
	}

	int get_size(const_spnode root){
		if (root == NULL) return 0; else return root->size;
	}

	void upd_size(pnode & root){
		if (root == NULL) return;
		root->size = 1 + get_size(root->left) + get_size(root->right);
	}

	void split(pnode root, const T & x, pnode & left, pnode & right){		
		if (root == NULL){
			left = NULL;
			right = NULL;						
		} else
			if (root->virt){
				return;
			}
			if (cmp_(x, root->value)) {
				*right = *root;			
				left->virt = true;
				check_virt_node(right->left); 
				pnode new_right = right->left.switch_to_mutable();
				split(new_right, x, left, new_right);			
				check_virt_node(right->left);
			} else {			
				*left = *root;			
				right->virt = true;
				check_virt_node(left->right);
				pnode new_left = left->right.switch_to_mutable();
				split(new_left, x, new_left, right);
				check_virt_node(left->right);
			}
			upd_size(left);
			upd_size(right);
	}

	void merge(pnode & root, pnode left, pnode right){
		if (left == NULL && right == NULL){
			root->virt = true;
		} else
			if (left == NULL || right == NULL){
				*root = *( (left == NULL) ? right : left) ;
			} else
				if (compare_priority(right, left) ){
					*root = *left;				
					pnode new_left = (root->right == NULL ? NULL : root->right.switch_to_mutable());
					merge(new_left, new_left, right);
					check_virt_node(root->right);
				} else {
					*root = *right;				
					pnode new_right = (root->left == NULL ? NULL : root->left.switch_to_mutable());
					merge(new_right, left, new_right);
					check_virt_node(root->left);
				}
				upd_size(root);
	}

};



#pragma once
#include <vector>
#include <algorithm>
#include <cstdio>
#include <iostream>


template<class T>
struct node{
	
	typedef node<T>* pnode;

	class const_spnode : public std::shared_ptr<const node<T> > {
    public:

    pnode clone() {
		if (this->unique()) {
        return const_cast<pnode>((this->get()));
		} else 
			if (*this == NULL) {
				pnode my_clone = NULL;
				this->base_class::operator=(static_cast<base_class>(my_clone));
				return my_clone;
			} else {
				pnode my_clone(new node(**this));
				this->base_class::operator=(static_cast<base_class>(my_clone));
				return my_clone;
			}
	}
		
	void make_leaf(const T & value) {
		pnode new_node = new node(value);
		this->reset(new_node);
	}

	void nullify(){
		pnode new_node = NULL;
		this->reset(new_node);
	}

    private:
    typedef std::shared_ptr<const node<T> > base_class;
  };


	static const int MAXPRIORITY =	(int)1e+6;

	T value;
	int priority;
	int size;
	const_spnode left;
	const_spnode right;

	explicit node (const T &value) : value(value), 
						priority(rand()%MAXPRIORITY), 
						size(1){
		pnode new_left = left.clone();
		new_left = NULL;
		pnode new_right = right.clone();
		new_right = NULL;
		
	}

	explicit node (const_spnode nod) : value(nod->value) {
		priority = nod->priority;
		size = nod->size;
		left = nod->left;
		right = nod->right;
	}

/*	explicit node (spnode nod) : value(nod->value) {
		priority = nod->priority;
		size = nod->size;
		left = nod->left;
		right = nod->right;
	}
	*/
	explicit node (pnode nod) : value(nod->value) {
		priority = nod->priority;
		size = nod->size;
		left = nod->left;
		right = nod->right;
	}

	void copy_pnode(pnode n1){		
		this->value = n1->value;		
		this->left = n1->left;
		this->right = n1->right;
		this->priority = n1->priority;
		this->size = n1->size;
	}

	pnode operator=(pnode n1){
		if (this == NULL){
			this = new node<T>(n1);
			return this;
		} else
			this->value = n1->value;		
			this->left = n1->left;
			this->right = n1->right;
			this->priority = n1->priority;
			this->size = n1->size;
			return this;
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
	static const int UNREAL_VALUE = -999999999;

public:
	persistent_treap() : root_(){		
	}

	persistent_treap (const persistent_treap<T, Comparator> & base_treap):
		root_(base_treap.root_),
		cmp_(base_treap.cmp_){
	}

	void operator =(const persistent_treap<T, Comparator> & base_treap) {
		root_ = base_treap->root;		
		cmp_ = base_treap->cmp_;
	}

	void push(const T &value){
		if (exist(value)) return;
		if (root_ == NULL) {			
			root_.make_leaf(value);
			return;
		}
		pnode new_root = root_.clone(); 		
		const_spnode const_new_node_ptr;
		const_new_node_ptr.make_leaf(value);
		pnode new_node_ptr = const_new_node_ptr.clone();
		insert(new_root, new_node_ptr);		
	}

	bool pop(const T & value){
		if (!exist_(root_, value)) return false;
		pnode new_root = root_.clone();
		erase(new_root, value);
		return true;
	}	

	bool exist(const T & value){
		return exist_(root_, value);
	}

	T get_kth(int value){
		return get_kth_(root_, value);
	}

	int size(){
		return root_->size;
	}

/*	treap Union(treap t1, treap t2){

	}
	*/
private:
	void construct_root(const T& value) {
		root_->make_leaf(value);
	}

	void check_null(const_spnode & nod){
		if (nod == NULL) nod.make_leaf(UNREAL_VALUE); else
		if (nod->value == UNREAL_VALUE) nod.nullify();
		return;		
	}

	void check_children(pnode & nod){
		if (nod->left == NULL) nod->left.make_leaf(UNREAL_VALUE); else
			if (nod->left->value == UNREAL_VALUE) nod->left.nullify();
		if (nod->right == NULL) nod->right.make_leaf(UNREAL_VALUE); else
			if (nod->right->value == UNREAL_VALUE) nod->right.nullify();
		return;
	}
	void insert(pnode & root, pnode & new_node){
		if (root->value == UNREAL_VALUE) {	
			root->copy_pnode(new_node);
		} else					
			if (compare_priority(root, new_node) ){	
				check_children(new_node);				
				pnode new_left = new_node->left.clone();				//нельзя клону присваивать null, 
				pnode new_right = new_node->right.clone();				//создаю мнимую ноду, потому если ей
				split (root, new_node->value, new_left, new_right);		//ничего не присваивается, удаляю
				check_children(new_node);				
				//root = new_node;				
				root->copy_pnode(new_node);				
			} else {				
				check_children(root);
				
				pnode new_left = root->left.clone();
				pnode new_right = root->right.clone();
				insert ( (cmp_(new_node->value, root->value) ? new_left : new_right), new_node);				

				check_children(root);				
			}
		upd_size(root);
	}

	T get_kth_(const_spnode root, int x){
		int left_size = (root->left == NULL ? 0 : root->left->size);
		int right_size = (root->right == NULL ? 0 : root->right->size);
		if (left_size == x-1) return root->value;
		if (left_size >= x) return get_kth_(root->left, x);
		return get_kth_(root->right, x - left_size - 1);
	}

	bool exist_(const_spnode root, const T & x){
		if (root == NULL) return false;
		if (root->value == x) return true;
		if (cmp_(x, root->value)) return exist_(root->left, x); else
								return exist_(root->right, x);
	}

	bool compare_priority(pnode n1, pnode n2){
		if (n1->priority < n2->priority ||
			(n1->priority == n2->priority && rand()%2 == 0) )
			return true; else
			return false;
	}

	void erase(pnode & root, const T & value){
		if (!cmp_(root->value, value) && !cmp(value, root->value)){
			pnode new_left = root->left.clone();
			pnode new_right = root->right.clone();
			merge(root, root->left->clone(), root->right->clone());
			check_null(root);
		} else
			if (cmp_(val,root->value)) pnode new_root = root->left.clone(); 
				else pnode new_root = root->right.clone();
			erase (new_root, val);
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
		if (cmp_(x, root->value)) {
			right->copy_pnode(root);
			check_null(right->left); 
			pnode new_right = right->left.clone();
			split(root->left.clone(), x, left, new_right);			
			check_null(right->left);
		} else {			
			left->copy_pnode(root);
			check_null(left->right);
			pnode new_left = left->right.clone();
			split(root->right.clone(), x, new_left, right);
			check_null(left->right);
		}
		upd_size(left);
		upd_size(right);
	}

	void merge(pnode & root, pnode left, pnode right){
		if (left == NULL && right == NULL){
			root->value = UNREAL_VALUE;
		}
		if (left == NULL || right == NULL){
			root->copy_pnode( ( (left == NULL) ? left : right) );
		} else
			if (compare_priority(right, left) ){
					root.copy_pnode(left);
					check_null(root->right);
					pnode new_left = root->right.clone();
					merge(new_left, new_left, right);
					check_null(root->right);
			} else {
				root = right;
				check_null(root->left);
				pnode new_right = root->left.clone();
				merge(new_right, left, new_right);
				check_null(root->left);
			}
		upd_size(root);
	}

};



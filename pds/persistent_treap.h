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


	static const int MAXPRIORITY =	(int)1e+6;

	T value;
	int priority;
	int size;
	bool virt;
	const_spnode left;
	const_spnode right;

	explicit node (const T &value) : value(value), 
						priority(rand()%MAXPRIORITY), 
						size(1),
						virt(false){
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
		virt = nod->virt;
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
		virt = nod->virt;
	}

	void copy_pnode(pnode n1){		
		const_spnode stay_right = this->right;
		const_spnode stay_left = this->left;
		this->value = n1->value;		
		this->left = n1->left;
		this->right = n1->right;
		this->priority = n1->priority;
		this->size = n1->size;
		this->virt = n1->virt;
		stay_right.nullify();
		stay_left.nullify();
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
		pnode new_root = root_.clone(); 		
		const_spnode const_new_node_ptr;
		const_new_node_ptr.make_leaf(value);
		pnode new_node_ptr = const_new_node_ptr.clone();
		insert(new_root, new_node_ptr);		
		return true;
	}

	void clear(){
		root_.nullify();
	}

	bool pop(const T & value){
		if (!exist_(root_, value)) return false;
		pnode new_root = root_.clone();
		erase(new_root, value);
		if (new_root->virt) clear();
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

	void print_tree(){
		print_node(root_);
	}

/*	treap Union(treap t1, treap t2){

	}
	*/
private:
	void construct_root(const T& value) {
		root_->make_leaf(value);
	}
	
	void print_node(const const_spnode & root){
		if (root == NULL) std::cout<< "null" << std::endl; else
		{
			std::cout << root->value << " " << root->priority << std::endl;
			print_node(root->left);
			print_node(root->right);
		}
	}

	void check_null(const_spnode & nod){
		if (nod == NULL) {
			nod.make_virt_leaf(T());
		} else
		if (nod->virt) nod.nullify();
		return;		
	}

	void check_children(pnode & nod){
		if (nod->left == NULL) nod->left.make_virt_leaf(T()); else
			if (nod->left->virt) nod->left.nullify();
		if (nod->right == NULL) nod->right.make_virt_leaf(T()); else
			if (nod->right->virt) nod->right.nullify();
		return;
	}

	void check_children_null(pnode & nod){
		if (nod->left != NULL && nod->left->virt) nod->left.nullify();
		if (nod->right != NULL && nod->right->virt) nod->right.nullify();
	}

	void insert(pnode & root, pnode & new_node){
		if (root->virt) {	
			root->copy_pnode(new_node);
		} else					
			if (compare_priority(root, new_node) ){	
				check_children(new_node);				
				pnode new_left = new_node->left.clone();				
				pnode new_right = new_node->right.clone();				
				split (root, new_node->value, new_left, new_right);		
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
		if (!cmp_(root->value, value) && !cmp_(value, root->value)){
			pnode new_left = root->left.clone();
			pnode new_right = root->right.clone();
			merge(root, new_left, new_right);
			check_children_null(root);
		} else{
			pnode new_root;
			if (cmp_(value,root->value)) new_root = root->left.clone(); 
				else new_root = root->right.clone();
			erase (new_root, value);
			check_children_null(root);
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
			right->copy_pnode(root);
			//left->value = NULL;
			left->virt = true;
			check_null(right->left); 
			pnode new_right = right->left.clone();
			split(new_right, x, left, new_right);			
			check_null(right->left);
		} else {			
			left->copy_pnode(root);
			//right->value = NULL;
			right->virt = true;
			check_null(left->right);
			pnode new_left = left->right.clone();
			split(new_left, x, new_left, right);
			check_null(left->right);
		}
		upd_size(left);
		upd_size(right);
	}

	void merge(pnode & root, pnode left, pnode right){
		if (left == NULL && right == NULL){
			root->virt = true;
		} else
		if (left == NULL || right == NULL){
			root->copy_pnode( ( (left == NULL) ? right : left) );
		} else
			if (compare_priority(right, left) ){
					root->copy_pnode(left);
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



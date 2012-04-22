#pragma once
#include <vector>
#include <algorithm>
#include <cstdio>
#include <iostream>

const int MAXPRIORITY =	1e+6;


template<class T>
struct node{

	typedef std::shared_ptr<node<T> > spnode;
	typedef std::shared_ptr<const node<T> > const_spnode;
	typedef node<T>* pnode;

	class const_spnode : public std::shared_ptr<const node<T> > {
	public:

		pnode clone() {
			pnode my_clone(new node(**this));
			this->spnode::operator=(static_cast<spnode>(my_clone));
			return my_clone;
		 }
	/*    void make_leaf(T value) {
			this->reset(new node(value));
		}
		*/
	private: 
	};


	T value;
	int priority;
	int size;
	spnode left;
	spnode right;
	explicit node (const T &value) : value(value), 
						priority(rand()%MAXPRIORITY), 
						size(1),
						left(NULL), 
						right(NULL){
		
	}

	explicit node (const_spnode nod) : value(nod->value) {
		priority = nod->priority;
		size = nod->size;
		left = nod->left;
		right = nod->right;
	}

	explicit node (spnode nod) : value(nod->value) {
		priority = nod->priority;
		size = nod->size;
		left = nod->left;
		right = nod->right;
	}

};




template<class T, class Comparator = std::less<T> >
class treap{
	typedef std::shared_ptr<node<T>> pnode;
private:	
	pnode root;
	Comparator cmp;
public:
	treap(){
		root = NULL;
	}


/*	explicit treap(T n, ...){
		root = NULL;
		T *p = &n;
		while (*p != 0){
			insert(*p);
			p++;
		}
	}	
	*/
	explicit treap(std::vector<T> init_val){
		root = NULL;
		for (int i = 0; i < init_val.size(); i++){
			insert(init_val[i]);
		}
	}

/*	explicit treap(const treap<T, Comparator> &tr) :
		root(tr.root){
	}
	*/
	void operator =(const treap<T, Comparator> &tr) {
		root = tr.root;		
	}

	void insert(const T &x){
		pnode new_node = pnode(new node<T>(x));
		if (exist(x)) return;
		_insert(root, new_node);
	}

	bool erase(const T & x){
		if (!_exist(root, x)) return false;
		_erase(root, x);
		return true;
	}	

	bool exist(const T & x){
		return _exist(root, x);
	}

	T getkth(int x){
		return _getkth(root, x);
	}

	int size(){
		return root->size;
	}

	treap Union(treap t1, treap t2){

	}

private:
	void _insert(pnode & root, pnode nod){
		if (root == NULL) {
			root = nod;
		} else
			if (compare_priority(root, nod) ){
				split (root, nod->value, nod->left, nod->right);
				root = nod;
			} else {
				_insert ( (cmp(nod->value, root->value) ? root->left : root->right), nod);				
			}
		upd_size(root);
	}

	T _getkth(pnode root, int x){
		//if (root->size < x) return false;

		int left_size = (root->left == NULL ? 0 : root->left->size);
		int right_size = (root->right == NULL ? 0 : root->right->size);
		if (left_size == x-1) return root->value;
		if (left_size >= x) return _getkth(root->left, x);
		return _getkth(root->right, x - left_size - 1);
	}

	bool _exist(pnode root, const T & x){
		if (root == NULL) return false;
		if (root->value == x) return true;
		if (cmp(x, root->value)) return _exist(root->left, x); else
								return _exist(root->right, x);
	}

	bool compare_priority(pnode n1, pnode n2){
		if (n1->priority < n2->priority ||
			(n1->priority == n2->priority && rand()%2 == 0) )
			return true; else
			return false;
	}

	void _erase(pnode & root, const T & val){
		if (!cmp(root->value, val) && !cmp(val, root->value)){
			merge(root, root->left, root->right);
		} else
			_erase ((cmp(root->value, val) ? root->left : root->right), val);
		upd_size(root);
	}

	int _size(pnode root){
		if (root == NULL) return 0; else return root->size;
	}

	void upd_size(pnode & root){
		if (root == NULL) return;
		root->size = 1 + _size(root->left) + _size(root->right);
	}

	void split(pnode root, const T & x, pnode & left, pnode & right){		
		if (root == NULL){
			left = NULL;
			right = NULL;						
		} else
		if (cmp(x, root->value)) {
			right = root;
			split(root->left, x, left, right->left);			
		} else {
			left = root;
			split(root->right, x, left->right, right);
		}
		upd_size(left);
		upd_size(right);
	}

	void merge(pnode & root, pnode left, pnode right){
		if (left == NULL || right == NULL){
			root = ( (left == NULL) ? left : right);
		} else
			if (compare_priority(right, left) ){   //if the priorities of the branches are the same, then merge randomly
					root = left;
					merge(root, root->right, right);
			} else {
				root = right;
				merge(root, left, root->left);
			}
		upd_size(root);
	}

};



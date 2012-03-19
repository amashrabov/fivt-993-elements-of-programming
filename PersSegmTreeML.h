#ifndef _SEGMENT_TREE_DPR-PAVLIN_
#define _SEGMENT_TREE_DPR-PAVLIN_

#include <iostream>
#include <cstdio>
#include <vector>
#include <cassert>
#include <functional>


template <typename T> struct scalMult
{
	T operator () (size_t n, T obj)
	{
		return n*obj;
	}
}
template <typename T, typename TSum = plus<T>, typename TScalMult =  ScalMult<T> >
class PersistantSegmentTree
{
	private: 
		
		class Vertex
		{
			private:
				init_()
				{
					deltaDef = false;
					valDef = false;
					leftV = rightV = NULL;		
				}
			public:
				T sum, delta, val;
				bool deltaDef, valDef;
				Vertex *leftV, *rightV;

				friend void PersistantSegmentTree::init_(Vertex* &root, int l, int r, const T &obj);				
				friend void PersistantSegmentTree::update_(int l, int r, T &obj); 
				friend T PersistantSegmentTree::findSum_(int l, int r); 

				
				Vertex(T &obj)
				{
					sum_ = obj;
					init_();
				}
				Vertex()
				{
					init_();					
				}
				Vertex(Vertex &v)
				{
					sum = v.sum;
					delta = v.delta;
					val = v.val;
					deltaDef = v.deltaDef;
					valDef = v.valDef;
					leftV = v.leftV;
					rightV = v.rightV;					
				}											
		}; 

		
		size_t size_;	
		Vertex* root_;
		int persCode_;		

		void init_(Vertex* &root, size_t l, size_t r, const T &obj)
		{
			assert(r < l);
			if (l == r)
			{
				root = new Vertex(obj);
			}
			else
			{
				root = new Vertex();
				int mid = (l+r)/2;
				init_(root->leftV, l, mid, obj);
				init_(root->rightV, mid+1, r, obj);
				root->sum = TSum(root->leftV->sum, root->rightV->sum);
			}  
		}
		T findSum_(Vertex* &root, size_t l, size_t r, size_t ql, size_t qr)
		{
			assert(l <= ql && qr <= r);
			if (ql == l && qr == r)
				return root->sum;
					
			if (root->valDef)
				return TScalMult(qr-ql+1, root->val);
			
			T ans;
			int mid = (l+r)/2;
			if (qr <= mid)
				ans = findSum_(root->leftV, l, mid, ql, qr);
			else if (ql > mid)
				ans = findSum_(root->rightV, mid+1, r, ql, qr);
			else
				ans = TSum(findSum_(root->leftV, l, mid, ql, mid), findSum_(root->rightV, mid+1, r, mid+1, qr));
			if (root->deltaDef)
				ans = TSum(ans, TScalMult(qr-ql+1, root->delta));
			return ans; 	 
		}
		// procedure making persistance
		void clone_(Vertex* &root) 
		{
			if (persCode == NOTPERSISTANT)
				return; 
			Vertex* tmp = new Vertex(root*);
			root = tmp;
		}
		// end of procedure
		void appOnVertex_(Vertex* &root, T &obj, int l, int r)
		{
				assert(root != NULL && l <= r);
				clone_(root); // for support of persistance
					
				root->deltaDef = false;
				root->valDef = true;
				root->val = obj;
				root->sum = TScalMult(r-l+1, obj);
		}
		void addOnVertex_(Vertex* &root, T &obj, int l, int r)
		{
				assert(root != NULL && l <= r);
				clone_(root); // for support of persistance
				
				if (root->deltaDef)
					root->val = TSum(root->val, obj);
				else if (root->deltaDef)
					root->delta = TSum(root->delta, obj);
				else
				{
					root->deltaDef = true;
					root->delta = obj;
				}
				root->sum = TSum(root->sum, TScalMult(r-l+1, obj));				
		}
		void pushDown_(Vertex* &root, int l, int r)
		{
			assert(root != NULL && root->leftV != NULL && root->rightV != NULL);
			int mid = (l+r)/2;			
			if (root->valDef)
			{
				root->valDef = false;
				appOnVertex(root->leftV, root->val, l, mid);
				appOnVertex(root->rightV, root->val, mid+1, r);
			}
			if (root->deltaDef)
			{
				root->deltaDef = false;
				addOnVertex(root->leftV, root->val, l, mid);
				addOnVertex(root->rightV, root->val, mid+1, r);
			}
			root->sum = TSum(root->leftV->sum, root->rightV->sum);
		}
		void update_(int operation, Vertex* &root, T &obj, int l, int r, int ql, int qr)
		{
			assert(l <= ql && qr <= r);
							
			if (ql == l && qr == r)
			{
				if (operation == APPROPRIATION)
					appOnVertex_(root, obj, l, r);			
				if (operation == ADDITION)
					addOnVertex_(root, obj, l, r);			
				return;
			}		
				
			clone_(root); // for support of persistance
			pushDown_(root, l, r);
			int mid = (l+r)/2;
			if (qr <= mid)
				update_(operation, root->leftV, obj, l, mid, ql, qr);
			else if (ql > mid)
				update_(operation, root->rightV, obj, mid+1, r, ql, qr);
			else
			{
				update_(operation, root->leftV, obj, l, mid, ql, mid);
				update_(operation, root->rightV, obj, mid+1, r, mid+1, qr);
			}
			
		}
	public:
		const int APPROPRIATION = 3124124;
		const int ADDITION = 1132;
		
		const int PERSISTANT = 1141414;    // for support of persistance
		const int NOTPERSISTANT = -1209;   // for support of persistance
		PersistantSegmentTree(int ind, size_t size, T &obj)
		{

			assert(ind == PERSISTANT || ind == NOTPERSISTANT);						
			persCode_ = ind;
					
			size_ = size;
			if (!size)
			{
				root_ = NULL;			
				return;
			}
			init_(root_, 0, r-1, obj);
		}
		
		size_t size()
		{
			return size_;
		}
		T findSum(size_t l, size_t r)
		{
			assert(l <= r && 0 <= l && r < size);
			return findSum_(root, 0, size_-1, l, r);
		}
		void appropriate(size_t l, size_t r, T &obj)
		{
			assert(0 <= l && l <= r && r < size);
			update_(APPROPRIATION, root_, obj, 0, size_-1, l, r);			
		}
		void add(size_t l, size_t r, T &obj)
		{
			assert(0 <= l && l <= r && r < size);				
			update_(ADDITION, root_, obj, 0, size_-1, l, r);			
		}
};

#endif

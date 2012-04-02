#ifndef  _SEGMENT_TREE_DPR-PAVLIN_
#define  _SEGMENT_TREE_DPR-PAVLIN_


// compile with --std=c++0x

#include <cassert>
#include <functional>
#include <memory>

template <typename T> struct ScalMult
{
	T operator () (size_t n, const T &obj) const
	{
		return n*obj;
	}
};

	
template <typename T, typename TSum = std::plus<T>, typename TScalMult =  ScalMult<T> >
class PersistantSegmentTree
{
	private: 
		TSum summator_;
		TScalMult multiplier_;
		//typedef PersistantSegmentTree<T,TSum,TScalMult> PST;
		
		class Vertex
		{
			//typedef Vertex<T, TSum, TScalMult> Vertex; 
			typedef std::shared_ptr<Vertex> ptrv;

			private:
				void init_()
				{
					deltaDef = false;
					valDef = false;
					leftV.reset();
					rightV.reset();		
				}
			public:
				T sum, delta, val;
				bool deltaDef, valDef;
				//Vertex *leftV, *rightV;
				ptrv leftV, rightV;
				//friend class PST;

				
				explicit Vertex(const T &obj)
				{
					sum = obj;
					init_();
				}
				Vertex()
				{
					init_();					
				}
				explicit Vertex (const Vertex &v)
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

		typedef std::shared_ptr<Vertex> ptrv;

		size_t size_;	
		std::shared_ptr<Vertex> root_;
		int persCode_;		

		void init_(ptrv &root, size_t l, size_t r, const T &obj)
		{
			assert(l <= r);
			if (l == r)
			{
				root = ptrv(new Vertex(obj));
			}
			else
			{
				root = ptrv(new Vertex());
				size_t mid = (l+r)/2;
				init_(root->leftV, l, mid, obj);
				init_(root->rightV, mid+1, r, obj);
				root->sum = summator_(root->leftV->sum, root->rightV->sum);
			}  
		}
		T findSum_(ptrv &root, size_t l, size_t r, size_t ql, size_t qr) const
		{
			assert(l <= ql && qr <= r);
			if (ql == l && qr == r)
				return root->sum;
					
			if (root->valDef)
				return multiplier_(qr-ql+1, root->val);

			T ans;
			int mid = (l+r)/2;
			if (qr <= mid)
				ans = findSum_(root->leftV, l, mid, ql, qr);
			else if (ql > mid)
				ans = findSum_(root->rightV, mid+1, r, ql, qr);
			else
				ans = summator_(findSum_(root->leftV, l, mid, ql, mid), findSum_(root->rightV, mid+1, r, mid+1, qr));
			if (root->deltaDef)
				ans = summator_(ans, multiplier_(qr-ql+1, root->delta));
			return ans; 	 
		}
		// procedure making persistance
		void clone_(ptrv &root) 
		{
			if (persCode_ == NOTPERSISTANT)
				return; 
			ptrv tmp(new Vertex(*root_) );
			root_ = tmp;
		}
		// end of procedure
		void assignOnVertexSegment_(ptrv &root, const T &obj, int l, int r)
		{
				assert(root != NULL && l <= r);
				clone_(root); // for support of persistance
					
				root->deltaDef = false;
				root->valDef = true;
				root->val = obj;
				root->sum = multiplier_(r-l+1, obj);
		}
		void addOnVertexSegment_(ptrv &root, const T &obj, int l, int r)
		{
				assert(root != NULL && l <= r);
				clone_(root); // for support of persistance
				
				if (root->valDef)
					root->val = summator_(root->val, obj);
				else if (root->deltaDef)
					root->delta = summator_(root->delta, obj);
				else
				{
					root->deltaDef = true;
					root->delta = obj;
				}
				root->sum = summator_(root->sum, multiplier_(r-l+1, obj));				
		}
		void pushDown_(ptrv &root, int l, int r)
		{
			assert(root != NULL && root->leftV != NULL && root->rightV != NULL);
			int mid = (l+r)/2;			
			if (root->valDef)
			{
				root->valDef = false;
				assignOnVertexSegment_(root->leftV, root->val, l, mid);
				assignOnVertexSegment_(root->rightV, root->val, mid+1, r);
			}
			if (root->deltaDef)
			{
				root->deltaDef = false;
				addOnVertexSegment_(root->leftV, root->val, l, mid);
				addOnVertexSegment_(root->rightV, root->val, mid+1, r);
			}
			root->sum = summator_(root->leftV->sum, root->rightV->sum);
		}
		void update_(int operation, ptrv &root, const T &obj, size_t l, size_t r, size_t ql, size_t qr)
		{
			assert(l <= ql && qr <= r);
							
			if (ql == l && qr == r)
			{
				if (operation == ASSIGNMENT)
					assignOnVertexSegment_(root, obj, l, r);			
				if (operation == ADDITION)
					addOnVertexSegment_(root, obj, l, r);			
				return;
			}		
				
			clone_(root); // for support of persistance
			pushDown_(root, l, r);
			size_t mid = (l+r)/2;
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
		static const int ASSIGNMENT = 3124124;
		static const int ADDITION = 1132;
		
		static const int PERSISTANT = 1141414;    // for support of persistance
		static const int NOTPERSISTANT = -1209;   // for support of persistance
		PersistantSegmentTree(bool persInd, size_t size, const T &obj)
		{

			//assert(persInd == PERSISTANT || persInd == NOTPERSISTANT);						
			if (persInd)			
				persCode_ = PERSISTANT;
			else
				persCode_ = NOTPERSISTANT;
			size_ = size;
			if (!size)
			{	
				Vertex* uk = NULL;
				root_ = ptrv(uk);			
				return;
			}
			init_(root_, 0, size_-1, obj);
		}
		
		size_t size()
		{
			return size_;
		}
		T findSum(size_t l, size_t r) 
		{
			assert(l <= r && 0 <= l && r < size_);
			size_t l1 = 0;
			size_t r1 = size_-1;
			return findSum_(root_, l1, r1, l, r);
		}
		void assign(size_t l, size_t r, const T &obj)
		{
			assert(0 <= l && l <= r && r < size_);
			update_(ASSIGNMENT, root_, obj, 0, size_-1, l, r);			
		}
		void add(size_t l, size_t r, const T &obj)
		{
			assert(0 <= l && l <= r && r < size_);				
			update_(ADDITION, root_, obj, 0, size_-1, l, r);			
		}

};

#endif

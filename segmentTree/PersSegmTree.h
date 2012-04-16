#ifndef  _SEGMENT_TREE_DPR-PAVLIN_
#define  _SEGMENT_TREE_DPR-PAVLIN_


// compile with --std=c++0x

#include <cassert>
#include <functional>
#include <memory>
#include <iostream>

namespace stlext
{

template <typename T> struct ScalMult
{
	T operator () (size_t n, const T &obj) const
	{
		return n*obj;
	}
};

struct Interval
{
private:
	size_t left_, right_;
public:	
	Interval(size_t left, size_t right)
	:left_(left), right_(right)
	{
		assert(left_ <= right);
	}
	size_t left() const
	{
		return left_;
	}
	size_t right() const
	{
		return right_;
	}
	size_t length() const
	{
		return right_-left_+1;
	}
	Interval leftHalf() const
	{
		return Interval(left_, (left_+right_)/2);
	}
	Interval rightHalf() const
	{
		return Interval((left_+right_)/2+1, right_);
	}
	bool isIntersect(Interval &interval)
	{
		return !(right_ < interval.left() && interval.right() < left_);
	}
	bool isIn(Interval &interval)
	{
		return interval.left() <= left_ && right_ <= interval.right();
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
			typedef std::shared_ptr<Vertex> ptrvertex;

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
				ptrvertex leftV, rightV;
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
				void writeVertex(std::ostream& fout)		
				{
					fout << "{sum = " << sum << "; val = ";
					if (valDef)
						fout << val;
					else
						fout << "NA";
					fout << "; delta = ";
					if (deltaDef)
						fout << delta;
					else
						fout << "NA";
					fout << "}";
				}										
		}; 

		typedef std::shared_ptr<Vertex> ptrvertex;

		size_t size_;	
		ptrvertex root_;
		bool isPersistant_;		
		
		//friend class Adder, Assigner, SumFinder;
		
		void init_(ptrvertex &vert, Interval interval, const T &obj)
		{
			if (interval.length() == 1)
			{
				vert = ptrvertex(new Vertex(obj));
			}
			else
			{
				vert = ptrvertex(new Vertex());
				init_(vert->leftV, interval.leftHalf(), obj);
				init_(vert->rightV, interval.rightHalf(), obj);
				vert->sum = summator_(vert->leftV->sum, vert->rightV->sum);
			}  
		}
		void clone_(ptrvertex &vert) 
		{
			if (!isPersistant_)
				return; 
			//std::cout << "CLONE MADE\n";
			ptrvertex tmp(new Vertex(*vert) );
			vert = tmp;
		}
		void pushDown_(ptrvertex &vert, const Interval &interval)
		{
			assert(vert != NULL && vert->leftV != NULL && vert->rightV != NULL);
			if (vert->valDef)
			{
				vert->valDef = false;
				assignOnVertexSegment_(vert->leftV, vert->val, interval.leftHalf());
				assignOnVertexSegment_(vert->rightV, vert->val, interval.rightHalf());
			}
			if (vert->deltaDef)
			{
				vert->deltaDef = false;
				addOnVertexSegment_(vert->leftV, vert->val, interval.leftHalf());
				addOnVertexSegment_(vert->rightV, vert->val, interval.rightHalf());
			}
			vert->sum = summator_(vert->leftV->sum, vert->rightV->sum);
		}
		template <typename ACTIONTYPE>
		void goDfs(ptrvertex &vert, Interval curInt, Interval &queryInt, ACTIONTYPE &action)
		{
			if (!curInt.isIntersect(queryInt))
				return;
			if (curInt.isIn(queryInt))
			{
				action(vert, queryInt);
				return;
			}
			goDfs(vert->leftV, curInt.leftHalf(), queryInt, action);
			goDfs(vert->rightV, curInt.rightHalf(), queryInt, action);
		}		
		class Adder
		{
		public:
			T obj;
			TSum summator_;
			TScalMult multiplier_;
			PersistantSegmentTree<T, TSum, TScalMult>* myTree;

			Adder(const T &obj, PersistantSegmentTree<T, TSum, TScalMult>* myTree)
			:obj(obj), myTree(myTree)
			{}
			void operator () (ptrvertex &vert, Interval &interval)
			{
				assert(vert != NULL);
				myTree->clone_(vert); // for support of persistance
				
				if (vert->valDef)
					vert->val = summator_(vert->val, obj);
				else if (vert->deltaDef)
					vert->delta = summator_(vert->delta, obj);
				else
				{
					vert->deltaDef = true;
					vert->delta = obj;
				}
				vert->sum = summator_(vert->sum, multiplier_(interval.length(), obj));	
			} 
		};
		class Assigner
		{
		public:
			T obj;
			TSum summator_;
			TScalMult multiplier_;
			PersistantSegmentTree<T, TSum, TScalMult>* myTree;

			Assigner(const T &obj,  PersistantSegmentTree<T, TSum, TScalMult>* myTree)
			:obj(obj), myTree(myTree)
			{}
			void operator () (ptrvertex &vert, Interval &interval)
			{
				assert(vert != NULL);
				myTree->clone_(vert); // for support of persistance
					
				vert->deltaDef = false;
				vert->valDef = true;
				vert->val = obj;
				vert->sum = multiplier_(interval.length(), obj);
			}
		};
		class SumFinder
		{
		public:
			bool ansDef;
			T ans;		
			TSum summator_;
			TScalMult multiplier_;
	
			SumFinder()
			{
				ansDef = false;
			}
			void operator () (ptrvertex &vert, Interval &interval)
			{
				if (!ansDef)
				{
					ansDef = true;
					ans = vert->sum;
				}
				else
					ans = summator_(ans, vert->sum);
			}
		};
public:	
		PersistantSegmentTree(size_t size, const T &obj)
		:isPersistant_(true), size_(size)
		{

			//assert(persInd == PERSISTANT || persInd == NOTPERSISTANT);						
			if (!size)
			{	
				Vertex* uk = NULL;
				root_ = ptrvertex(uk);			
				return;
			}
			init_(root_, Interval(0, size_-1), obj);
		}
		PersistantSegmentTree(const PersistantSegmentTree<T, TSum, TScalMult> &tree)
		{
			size_ = tree.size_;	
			root_ = tree.root_;
			isPersistant_ = tree.isPersistant_;		
		}
		void operator = (const PersistantSegmentTree<T, TSum, TScalMult> &tree)
		{
			size_ = tree.size_;	
			root_ = tree.root_;
			isPersistant_ = tree.isPersistant_;		
		}
		
		size_t size()
		{
			return size_;
		}
		T findSum(size_t l, size_t r) 
		{
			assert(l <= r && 0 <= l && r < size_);
			SumFinder sumfinder;
			Interval query(l, r);	
			goDfs(root_, Interval(0, size_-1), query, sumfinder);	
			assert(sumfinder.ansDef);		
			return sumfinder.ans;
		}
		void assign(size_t l, size_t r, const T &obj)
		{
			assert(0 <= l && l <= r && r < size_);
			Assigner assigner(obj, this);
			Interval query(l, r);
			goDfs(root_, Interval(0, size_-1), query, assigner);				
		}
		void add(size_t l, size_t r, const T &obj)
		{
			assert(0 <= l && l <= r && r < size_);
			Adder adder(obj, this);
			Interval query(l, r);
			goDfs(root_, Interval(0, size_-1), query, adder);						
		}
};

}

#endif

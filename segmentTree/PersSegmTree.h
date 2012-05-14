#ifndef  _SEGMENT_TREE_DPR-PAVLIN_
#define  _SEGMENT_TREE_DPR-PAVLIN_


// compile with --std=c++0x
// ЗАГОНЯЕМ ВСЕ Т в PTRT
// ЗАГОНЯЕМ ВСЕ Т в PTRT
// ЗАГОНЯЕМ ВСЕ Т в PTRT
// ЗАГОНЯЕМ ВСЕ Т в PTRT
// ЗАГОНЯЕМ ВСЕ Т в PTRT
// ЗАГОНЯЕМ ВСЕ Т в PTRT
// ЗАГОНЯЕМ ВСЕ Т в PTRT
// ЗАГОНЯЕМ ВСЕ Т в PTRT
// ЗАГОНЯЕМ ВСЕ Т в PTRT
// ЗАГОНЯЕМ ВСЕ Т в PTRT
// ЗАГОНЯЕМ ВСЕ Т в PTRT
// ЗАГОНЯЕМ ВСЕ Т в PTRT
// ЗАГОНЯЕМ ВСЕ Т в PTRT
// ЗАГОНЯЕМ ВСЕ Т в PTRT
// ЗАГОНЯЕМ ВСЕ Т в PTRT
// ЗАГОНЯЕМ ВСЕ Т в PTRT
// ЗАГОНЯЕМ ВСЕ Т в PTRT
// ЗАГОНЯЕМ ВСЕ Т в PTRT
// ЗАГОНЯЕМ ВСЕ Т в PTRT
// ЗАГОНЯЕМ ВСЕ Т в PTRT
// ЗАГОНЯЕМ ВСЕ Т в PTRT
// ЗАГОНЯЕМ ВСЕ Т в PTRT
// ЗАГОНЯЕМ ВСЕ Т в PTRT

#include <cassert>
#include <functional>
#include <memory>
#include <iostream>

namespace stlext
{

template <typename T>
struct ScalMult
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
		return !(right_ < interval.left() || interval.right() < left_);
	}
	bool isIn(Interval &interval)
	{
		return interval.left() <= left_ && right_ <= interval.right();
	}
	void writeVertex(std::ostream &fout) const
	{
		fout << "[" << left_ << " " << right_ << "]";
	}	
};
std::ostream& operator << (std::ostream &fout, const Interval &interval)
{
	interval.writeVertex(fout);
}

template <typename T> 
class Vertex
{
	typedef std::shared_ptr<Vertex<T> > ptrvertex;
	typedef std::shared_ptr<T> ptrt;
	private:
		void init_()
		{
			deltaDef = false;
			valDef = false;
			leftV.reset();
			rightV.reset();		
		}
	public:
		ptrt sum, delta, val;
		bool deltaDef, valDef;
		ptrvertex leftV, rightV;
		
		explicit Vertex(const T &obj)
		{
			//sum = obj;
			sum = ptrt(new T(obj));
			init_();
		}
		Vertex()
		{
			init_();					
		}	
		void writeVertex(std::ostream& fout)		
		{
			fout << "{sum = " << *sum << "; val = ";
			if (valDef)
				fout << *val;
			else
				fout << "NA";
			fout << "; delta = ";
			if (deltaDef)
				fout << *delta;
			else
				fout << "NA";
			fout << "}";
		}										
}; 
	
template <typename T> 
void initPtrt(std::shared_ptr<T> &p, const T &obj)
{
	p = std::shared_ptr<T>(new T(obj));
} 
template <typename T>
void clone(std::shared_ptr<Vertex<T> > &vert) 
{
	
	std::shared_ptr<Vertex<T> > tmp(new Vertex<T>(*vert) );
	vert = tmp;
}
template <typename T, typename TSum = std::plus<T>, typename TScalMult =  ScalMult<T> >
class PersistantSegmentTree
{
	private: 
		TSum summator_;
		TScalMult multiplier_;
		
		typedef std::shared_ptr<Vertex<T> > ptrvertex;
		typedef std::shared_ptr<T> ptrt;
		
		void refindSum_(ptrvertex &vert)
		{
			assert(vert != NULL);
			
			initPtrt(vert->sum, summator_(*(vert->leftV->sum), *(vert->rightV->sum)));
		}
		size_t size_;	
		ptrvertex root_;
		void init_(ptrvertex &vert, Interval interval, const T &obj)
		{
			if (interval.length() == 1)
			{
				vert = ptrvertex(new Vertex<T>(obj));
			}
			else
			{
				vert = ptrvertex(new Vertex<T>());
				init_(vert->leftV, interval.leftHalf(), obj);
				init_(vert->rightV, interval.rightHalf(), obj);
				refindSum_(vert);
			}  
		}
		
		void assignOnVertexSegment_(ptrvertex &root, const T &obj, Interval interval)
		{
				assert(root != NULL);
				clone(root); // for support of persistance
					
				root->deltaDef = false;
				root->valDef = true;
				initPtrt(root->val, obj);
				initPtrt(root->sum, multiplier_(interval.length(), obj));
		}
		void addOnVertexSegment_(ptrvertex &root, const T &obj, Interval interval)
		{
				assert(root != NULL);
				clone(root); // for support of persistance
				
				if (root->valDef)
					root->val = ptrt(new T(summator_(*(root->val), obj)));
				else if (root->deltaDef)
					root->delta = ptrt(new T(summator_(*(root->delta), obj)));
				else
				{
					root->deltaDef = true;
					root->delta = ptrt(new T(obj));
				}
				initPtrt(root->sum, summator_(*(root->sum), multiplier_(interval.length(), obj)));				
		}
		void pushDown_(ptrvertex &vert, const Interval &interval)
		{
			assert(vert != NULL && vert->leftV != NULL && vert->rightV != NULL);
			clone(vert);
			clone(vert->leftV);
			clone(vert->rightV);
			if (vert->valDef)
			{
				vert->valDef = false;
				assignOnVertexSegment_(vert->leftV, *(vert->val), interval.leftHalf());
				assignOnVertexSegment_(vert->rightV, *(vert->val), interval.rightHalf());
			}
			if (vert->deltaDef)
			{
				vert->deltaDef = false;
				addOnVertexSegment_(vert->leftV, *(vert->delta), interval.leftHalf());
				addOnVertexSegment_(vert->rightV, *(vert->delta), interval.rightHalf());
			}
			refindSum_(vert);
		}
		template <typename ACTIONTYPE>
		void goDfs(ptrvertex &vert, Interval curInt, Interval &queryInt, ACTIONTYPE &action)
		{
			if (!curInt.isIntersect(queryInt))
				return;
			clone(vert);			
			if (curInt.isIn(queryInt))
			{
				action(vert, curInt);
				return;
			}
			pushDown_(vert, curInt);						
			goDfs(vert->leftV, curInt.leftHalf(), queryInt, action);
			goDfs(vert->rightV, curInt.rightHalf(), queryInt, action);
			refindSum_(vert);
			//vert->sum = summator_(vert->leftV->sum, vert->rightV->sum);
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
				myTree->addOnVertexSegment_(vert, obj, interval);	
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
				myTree->assignOnVertexSegment_(vert, obj, interval);	
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
					ans = *(vert->sum);
				}
				else
		
					ans = summator_(ans, *(vert->sum));
			}
		};
		void writeDfs_(std::ostream &fout, ptrvertex &root, int depth, int l, int r)
		{
			int mid = (l+r)/2;
			if (l != r)
				writeDfs_(fout, root->leftV, depth+1, l, mid);
			for (int i = 0; i < depth; i++)
				fout << "------";
			fout << l << " " << r << " ";
			root->writeVertex(fout);
			fout << "\n";
			if (l != r)
				writeDfs_(fout, root->rightV, depth+1, mid+1, r);
		}
public:	
		PersistantSegmentTree(size_t size, const T &obj)
		:size_(size)
		{
			if (!size)
			{	
				Vertex<T>* uk = NULL;
				root_ = ptrvertex(uk);			
				return;
			}
			init_(root_, Interval(0, size_-1), obj);
		}
		
		PersistantSegmentTree(const PersistantSegmentTree<T, TSum, TScalMult> &tree)
		{
			size_ = tree.size_;	
			root_ = tree.root_;	
		}
		void operator = (const PersistantSegmentTree<T, TSum, TScalMult> &tree)
		{
			size_ = tree.size_;	
			root_ = tree.root_;		
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
		void writeTree(std::ostream &fout)
		{
			fout << "===================================================================\n";
			writeDfs_(fout, root_, 0, 0, size_-1);
			fout << "===================================================================\n";

		}
};

}

#endif

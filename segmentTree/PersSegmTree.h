#ifndef  _SEGMENT_TREE_DPR-PAVLIN_
#define  _SEGMENT_TREE_DPR-PAVLIN_


// compile with --std=c++0x

#include "pds_ptr.h"

#include <cassert>
#include <functional>
#include <memory>
#include <iostream>


namespace pds
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
void initPtrt(std::shared_ptr<T> &p, const T &obj)
{
	p = std::shared_ptr<T>(new T(obj));
} 

template <typename T, typename TSum, typename TScalMult> 
class Vertex
{
	typedef Vertex<T, TSum, TScalMult> VertexT;
	typedef pds::pds_ptr<VertexT> ptrvertex;
	typedef std::shared_ptr<T> ptrt;
	private:
		void init_()
		{
			deltaDef = false;
			valDef = false;
			leftV = NULL;
			rightV = NULL;		
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
		void assignOnVertexSegment(const T &obj, Interval interval)
		{
				deltaDef = false;
				valDef = true;
				initPtrt(val, obj);
				TScalMult multiplier_;
				initPtrt(sum, multiplier_(interval.length(), obj));
		}
		void addOnVertexSegment(const T &obj, Interval interval)
		{
				TSum summator_;
				TScalMult multiplier_;
				if (valDef)
					val = ptrt(new T(summator_(*(val), obj)));
				else if (deltaDef)
					delta = ptrt(new T(summator_(*(delta), obj)));
				else
				{
					deltaDef = true;
					delta = ptrt(new T(obj));
				}
				initPtrt(sum, summator_(*(sum), multiplier_(interval.length(), obj)));				
		}
		void refindSum()
		{
			VertexT *ukleft = leftV.switch_to_mutable();
			VertexT *ukright = rightV.switch_to_mutable();
			TSum summator_;
			initPtrt(sum, summator_(*(ukleft->sum), *(ukright->sum)));
		}
		/*void writeVertex(std::ostream& fout)		
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
		}		*/								
}; 
/*template <typename T>
void clone(std::shared_ptr<Vertex<T> > &vert) 
{
	
	std::shared_ptr<Vertex<T> > tmp(new Vertex<T>(*vert) );
	vert = tmp;
}*/
template<typename T, typename VertexT>
class Adder
{
public:
	T obj;
	
	Adder(const T &obj)
	:obj(obj)
	{}
	void operator () (pds::pds_ptr<VertexT> &vert, Interval &interval)
	{
		VertexT *uk = vert.switch_to_mutable();
		uk->addOnVertexSegment(obj, interval);
	} 
};
template<typename T, typename VertexT>
class Assigner
{
public:
	T obj;
	
	Assigner(const T &obj)
	:obj(obj)
	{}
	void operator () (pds::pds_ptr<VertexT> &vert, Interval &interval)
	{
		VertexT *uk = vert.switch_to_mutable();
		uk->assignOnVertexSegment(obj, interval);	
	}
};
template<typename T, typename TSum, typename VertexT>
class SumFinder
{
private:
	bool ansDef;
	T ans;		
	TSum summator_;	
public:
	SumFinder()
	{
		ansDef = false;
	}
	void operator () (pds::pds_ptr<VertexT> &vert, Interval &interval)
	{
		VertexT *uk = vert.switch_to_mutable();		
		if (!ansDef)
		{
			ansDef = true;
			ans = *(uk->sum);
		}
		else

			ans = summator_(ans, *(uk->sum));
	}
	T getAns()
	{
		return ans;
	}
};
template <typename T, typename TSum = std::plus<T>, typename TScalMult =  ScalMult<T> >
class PersistantSegmentTree
{
	private: 
		TSum summator_;
		TScalMult multiplier_;
		typedef Vertex<T, TSum, TScalMult> VertexT;
		typedef pds::pds_ptr<VertexT> ptrvertex;
		typedef std::shared_ptr<T> ptrt;

		
		
		size_t size_;	
		ptrvertex root_;
		void init_(ptrvertex &vert, Interval interval, const T &obj)
		{
			if (interval.length() == 1)
			{
				vert = (new VertexT(obj));
			}
			else
			{
				VertexT *uk = new VertexT();
				vert = new VertexT();
				init_(uk->leftV, interval.leftHalf(), obj);
				init_(uk->rightV, interval.rightHalf(), obj);
				uk->refindSum();				
				vert = uk;
			}  
		}
		
		
		void pushDown_(ptrvertex &vert, const Interval &interval)
		{
			assert(vert != NULL);
			VertexT *uk = vert.switch_to_mutable();
			assert(uk->leftV != NULL && uk->rightV != NULL);
			VertexT *ukleft = uk->leftV.switch_to_mutable();
			VertexT *ukright = uk->rightV.switch_to_mutable();
			
			if (uk->valDef)
			{
				uk->valDef = false;
				ukleft->assignOnVertexSegment(*(vert->val), interval.leftHalf());
				ukright->assignOnVertexSegment(*(vert->val), interval.rightHalf());
			}
			if (uk->deltaDef)
			{
				uk->deltaDef = false;
				ukleft->addOnVertexSegment(*(vert->delta), interval.leftHalf());
				ukright->addOnVertexSegment(*(vert->delta), interval.rightHalf());
			}
			uk->refindSum();
		}
		template <typename ACTIONTYPE>
		void goDfs(ptrvertex &vert, Interval curInt, Interval &queryInt, ACTIONTYPE &action)
		{
			if (!curInt.isIntersect(queryInt))
				return;
			if (curInt.isIn(queryInt))
			{
				action(vert, curInt);
				return;
			}
			pushDown_(vert, curInt);
			VertexT *uk = vert.switch_to_mutable();						
			goDfs(uk->leftV, curInt.leftHalf(), queryInt, action);
			goDfs(uk->rightV, curInt.rightHalf(), queryInt, action);
			uk->refindSum();			
			//vert->sum = summator_(vert->leftV->sum, vert->rightV->sum);
		}		
		
		/*void writeDfs_(std::ostream &fout, ptrvertex &root, int depth, int l, int r)
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
		}*/
public:	
		PersistantSegmentTree(size_t size, const T &obj)
		:size_(size)
		{
			if (!size)
			{	
				VertexT* uk = NULL;
				root_ = uk;			
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
			SumFinder<T, TSum, VertexT> sumfinder;
			Interval query(l, r);	
			goDfs(root_, Interval(0, size_-1), query, sumfinder);	
			//assert(sumfinder.ansDef);		
			return sumfinder.getAns();
		}
		void assign(size_t l, size_t r, const T &obj)
		{
			assert(0 <= l && l <= r && r < size_);
			Assigner<T, VertexT> assigner(obj);
			Interval query(l, r);
			goDfs(root_, Interval(0, size_-1), query, assigner);				
		}
		void add(size_t l, size_t r, const T &obj)
		{
			assert(0 <= l && l <= r && r < size_);
			Adder<T, VertexT> adder(obj);
			Interval query(l, r);
			goDfs(root_, Interval(0, size_-1), query, adder);						
		}
		/*void writeTree(std::ostream &fout)
		{
			fout << "===================================================================\n";
			writeDfs_(fout, root_, 0, 0, size_-1);
			fout << "===================================================================\n";

		}*/
};

}

#endif

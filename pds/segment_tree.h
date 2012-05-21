#ifndef _SEGMENT_TREE_H_
#define _SEGMENT_TREE_H_

#include <cassert>
#include <functional>
#include <memory>

#include "pds_ptr.h"

namespace pds
{

template <typename T>
struct scal_mult
{
  T operator () (size_t n, const T &obj) const
  {
    return n * obj;
  }
};

struct interval
{
 private:
  size_t left_, right_;
 public:  
  interval(size_t left, size_t right)
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

  interval leftHalf() const
  {
    return interval(left_, (left_+right_)/2);
  }

  interval rightHalf() const
  {
    return interval((left_+right_)/2+1, right_);
  }

  bool isIntersect(interval &interval)
  {
    return !(right_ < interval.left() || interval.right() < left_);
  }

  bool isIn(interval &interval)
  {
    return interval.left() <= left_ && right_ <= interval.right();
  }
};

template <typename T> 
void initPtrt(std::shared_ptr<T> &p, const T &obj)
{
  p = std::shared_ptr<T>(new T(obj));
} 

template <typename T, typename TSum, typename Tscal_mult> 
class Vertex
{
 private:
  typedef Vertex<T, TSum, Tscal_mult> VertexT;
  typedef pds::pds_ptr<VertexT> ptrvertex;
  typedef std::shared_ptr<T> ptrt;
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
  
  void assignOnVertexSegment(const T &obj, interval interval)
  {
    deltaDef = false;
    valDef = true;
    initPtrt(val, obj);
    Tscal_mult multiplier_;
    initPtrt(sum, multiplier_(interval.length(), obj));
  }
  
  void addOnVertexSegment(const T &obj, interval interval)
  {
    TSum summator_;
    Tscal_mult multiplier_;
    if (valDef)
    {
      val = ptrt(new T(summator_(*(val), obj)));
    }
    else if (deltaDef)
    {
      delta = ptrt(new T(summator_(*(delta), obj)));
    }
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
};
 
template<typename T, typename VertexT>
class Adder
{
 public:
  T obj;
  
  Adder(const T &obj)
  :obj(obj)
  {}
 
  void operator () (pds::pds_ptr<VertexT> &vert, interval &interval)
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
  void operator () (pds::pds_ptr<VertexT> &vert, interval &interval)
  {
    VertexT *uk = vert.switch_to_mutable();
    uk->assignOnVertexSegment(obj, interval);  
  }
};

template<typename T, typename TSum, typename VertexT>
class SumFinder
{
 public:
  SumFinder()
  {
    ansDef = false;
  }
  void operator () (pds::pds_ptr<VertexT> &vert, interval &interval)
  {
    VertexT *uk = vert.switch_to_mutable();    
    if (!ansDef)
    {
      ansDef = true;
      ans = *(uk->sum);
    }
    else
    {
      ans = summator_(ans, *(uk->sum));
    }
  }

  T getAns()
  {
    return ans;
  }

 private:
  bool ansDef;
  T ans;    
  TSum summator_;  
};

template <typename T, typename TSum = std::plus<T>, typename Tscal_mult =  scal_mult<T> >
class PersistantSegmentTree
{
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
    init_(root_, interval(0, size_-1), obj);
  }
    
  PersistantSegmentTree(const PersistantSegmentTree<T, TSum, Tscal_mult> &tree)
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
    interval query(l, r);  
    goDfs(root_, interval(0, size_-1), query, sumfinder);  
    return sumfinder.getAns();
  }

  void assign(size_t l, size_t r, const T &obj)
  {
    assert(0 <= l && l <= r && r < size_);
    Assigner<T, VertexT> assigner(obj);
    interval query(l, r);
    goDfs(root_, interval(0, size_-1), query, assigner);        
  }

  void add(size_t l, size_t r, const T &obj)
  {
    assert(0 <= l && l <= r && r < size_);
    Adder<T, VertexT> adder(obj);
    interval query(l, r);
    goDfs(root_, interval(0, size_-1), query, adder);            
  }

 private: 
  typedef Vertex<T, TSum, Tscal_mult> VertexT;
  typedef pds::pds_ptr<VertexT> ptrvertex;
  typedef std::shared_ptr<T> ptrt;

  size_t size_;  
  ptrvertex root_;
  void init_(ptrvertex &vert, interval interval, const T &obj)
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

  void pushDown_(ptrvertex &vert, const interval &interval)
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
  void goDfs(ptrvertex &vert, interval curInt, interval &queryInt, ACTIONTYPE &action)
  {
    if (!curInt.isIntersect(queryInt))
    {
      return;
    }
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
  }

  TSum summator_;
  Tscal_mult multiplier_;

};

}; // namespace pds

#endif

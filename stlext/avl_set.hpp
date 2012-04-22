#include <memory>
#include <functional>
#include <stack>
#include "avl_tree_node.hpp"
#include "copier.cpp"

namespace stlext {

// TODO the Comparator template would not be needed
/*template<class T, class Comparator>
class avl_tree_const_iterator: public std::iterator<std::forward_iterator_tag, AvlTreeNode<T, Comparator> > {
  public:
   typedef AvlTreeNode<T, Comparator> node_t;
   typedef typename node_t::node_csptr node_csptr;

   avl_tree_const_iterator(const avl_tree_const_iterator& it):
     root_(it.root_), node_(it.node_) {}

   avl_tree_const_iterator(const node_csptr& root, const node_csptr& node):
     root_(root), node_(node) {}

   avl_tree_const_iterator& operator++() {
     auto node = root_;

     while (true) {
       if (node.get() == NULL) {
         return;
 
         size_t index = (node->value < node_->value_)? 1 : 0;
         if (node->child_[index]->value_ == node_->value) {
           node_ = edge_leaf(node->child[index], 0);
           return;
         }
       }
     }

     return *this;
   }

   static avl_tree_const_iterator edge_leaf(const node_csptr& tree_root, int dir) {
     auto node = tree_root;
     
     if (node.get() == NULL)
       return node;

     while(node->child_[dir].get() != NULL) {
       node = node->child_[dir];
     }

     return node;
   }

   avl_tree_const_iterator operator++(int) {
     avl_tree_const_iterator tmp(*this); 
     operator++(); 
     return tmp;
   }

   bool operator==(const avl_tree_const_iterator& it) {
     return (root_ == it.root_ && node_ == it.node_);
   }

   bool operator!=(const avl_tree_const_iterator& it) {
     return !operator==(it);
   }

   const node_t& operator*() {
     return node_->value;
   }

  private:
   node_csptr root_;
   node_csptr node_;
};*/

template<class T, class Comparator = std::less<T> , class NodeCopier = Copier>
class AvlSet {
 public:
  typedef AvlTreeNode<T, Comparator> node_t;
  typedef typename node_t::node_ptr   node_ptr;
  typedef typename node_t::node_cptr  node_cptr;
  typedef typename node_t::node_sptr  node_sptr;
  typedef typename node_t::node_csptr node_csptr;

  AvlSet(): 
    size_(0) {}
  AvlSet(const AvlSet<T, Comparator, Copier> &set):
    size_(set.size_),
    root_(set.root_) {}
 
  
  bool insert(const T &value);
  bool remove(const T &value);
  bool contains(const T &value) const;

  size_t size() const;

 private:
  bool insert(const T &value, node_csptr &ref);
  bool remove(const T &value, node_csptr &ref);

  node_csptr root_;
  size_t size_;
  Comparator compare_;
};


template<class T, class Comparator, class NodeCopier>
bool AvlSet<T, Comparator, NodeCopier>::contains(const T& value) const {
  auto node = root_;

  while (true) {
    if (node.get() == NULL)
      return false;

    if (node->value_ == value)
      return true;
 
    size_t index = (compare_(value, node->value_))? 0 : 1;
    node = node->child_[index];    
  }
}

template<class T, class Comparator, class NodeCopier>
bool AvlSet<T, Comparator, NodeCopier>::insert(const T &value) {
  return insert(value, root_);
}


template<class T, class Comparator, class NodeCopier>
bool AvlSet<T, Comparator, NodeCopier>::insert(const T &value, node_csptr &ref) {
  // The place to insert is reached
  // Creationg new leaf here.
  if (ref.get() == NULL) {
    node_csptr& nonconst_ref = const_cast<node_csptr&>(ref);
    nonconst_ref = std::make_shared<const node_t>(value);
    ++size_;
    return true;
  }
    
  // The value already exists. No insertions are needed.
  if (ref->value_ == value)
    return false;
  
  size_t index = (compare_(value, ref->value_))? 0 : 1;
  
  // After insertion child_cpy will contain new 
  // root of child's subtree.
  node_csptr child_cpy = ref->child_[index];
  bool answ = insert(value, child_cpy);
  // If new child's root changed we must make a
  // copy of this node and change reference to
  // this subtree.
  if (child_cpy != ref->child_[index]) {
    // Copying the node
    ref = std::make_shared<const node_t>(NodeCopier::copy(*ref));
    // Updating the node
    const_cast<node_ptr>(ref.get())->child_[index] = child_cpy;
    // Balancing subtree of the node
    const_cast<node_ptr>(ref.get())->balance(ref);
  }

  return answ;
}


template<class T, class Comparator, class NodeCopier>
bool AvlSet<T, Comparator, NodeCopier>::remove(const T& value) {
  if (root_.get() == NULL) {
    return false;
  }

  bool ans = false;
  const_cast<node_ptr>(root_.get())->find_and_remove(root_, value, ans);

  if (ans) {
    --size_;
  }

  return ans;
}

template<class T, class Comparator, class NodeCopier>
bool AvlSet<T, Comparator, NodeCopier>::remove(const T &value, node_csptr &ref) {
  // We are in the node we want to delete
  // So we delete it.
  if (ref->value_ == value) {
    if (ref->height_ == 1) {
      ref = node_csptr();
    }
    else {  
      int index = (AvlTreeNode<T, Comparator>::get_factor(ref) > 0) ? 0 : 1;
      int dir =   (AvlTreeNode<T, Comparator>::get_factor(ref) > 0) ? 1 : 0;
      const_cast<node_ptr>(ref->child_[index].get())->remove_rec(ref->child_[index], ref->value_, dir);
    }

    return true;
  }
  
  // The node to delete not yet found
  // So continuing search.
  size_t index = (compare_(value, ref->value_))? 0 : 1; 
  if (ref->child_[index].get() != NULL) {
    remove(ref->child_[index], value, remove);
  }

  const_cast<node_ptr>(ref.get())->balance(ref);
}


template<class T, class Comparator, class NodeCopier>
size_t AvlSet<T, Comparator, NodeCopier>::size() const {
  return size_;
}




};

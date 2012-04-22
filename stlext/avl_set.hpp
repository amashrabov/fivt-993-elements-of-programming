#include <memory>
#include <functional>
#include <stack>
#include "avl_tree_node.hpp"
#include "copier.cpp"
#include "pseudo_copier.cpp"

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

template<class T, class Comparator = std::less<T> , class NodeCopier = PseudoCopier>
class AvlSet {
 public:
  typedef AvlTreeNode<T, Comparator> node_t;
  typedef typename node_t::node_ptr   node_ptr;
  typedef typename node_t::node_cptr  node_cptr;
  typedef typename node_t::node_sptr  node_sptr;
  typedef typename node_t::node_csptr node_csptr;

  void print() const {
    if (root_.get() != NULL) {
      std::cout << std::endl << "#PRINTING" << std::endl;
      root_->print();
    }
  }


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
  void replace(node_csptr &ref, T &value, int dir);

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
    // TODO do we need copier here if new shared pointer
    // is constructed anyways?
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

  bool ans = remove(value, root_);

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
    // If this is a list - everything is
    // very simple. Just deleting it.
    if (ref->height_ == 1) {
      ref = node_csptr();
    }
    // If this is not a list - a substitution
    // for this node is needed. Searching for
    // this substitution is initiated.
    else {  
      int index = (AvlTreeNode<T, Comparator>::get_factor(ref) > 0) ? 0 : 1;
      int dir = index ^ 0x01;
      // TODO the first time writing like this. If anything happens - check this line
      node_t new_node = Copier::copy(*ref);

      replace(new_node.child_[index], new_node.value_, dir);
      ref = std::make_shared<node_t>(new_node);
      new_node.balance(ref);
    }

    return true;
  }

  // The node to delete is not yet found
  // So continuing search.
  bool answ = false;
  size_t index = (compare_(value, ref->value_))? 0 : 1; 
  if (ref->child_[index].get() != NULL) {
    node_csptr child_cpy = ref->child_[index];
    answ = remove(value, child_cpy);

    // Checking if we have to update current 
    if (child_cpy != ref->child_[index]) {
      node_t new_node = Copier::copy(*ref);
      new_node.child_[index] = child_cpy;
      ref = std::make_shared<node_t>(new_node);
    }
  }

  const_cast<node_ptr>(ref.get())->balance(ref);
  return answ;
}


template<class T, class Comparator, class NodeCopier>
void AvlSet<T, Comparator, NodeCopier>::replace(node_csptr &ref, T &value, int dir) {
  int child_index = dir;

  // If no further movement in given direction
  // is possible - we must perform some actions
  if (ref->child_[dir].get() == NULL) {
    // The value is not needed here
    value = std::move(ref->value_);

    // If the replacement is a leaf
    // just forgetting about it.
    if (ref->height_ == 1) {
      ref = node_csptr();
    }
    // If this is not a leaf then the 
    // node only has one child.We put
    // this child into parentr's place.
    else {
      // This reverts the last bit of dir.
      // It converts 1 to 0 and 0 to 1.
      ref = ref->child_[dir ^= 0x01];
    }

    // Everything we wanted to do is done
    return;
  }

  // Going further till we reach the bottom.
  // We have to update all the nodes cause
  // replacing element will always be deleted.
  node_t new_node = Copier::copy(*ref);
  replace(new_node.child_[dir], value, dir);
  ref = std::make_shared<node_t>(new_node);
  new_node.balance(ref);
}


template<class T, class Comparator, class NodeCopier>
size_t AvlSet<T, Comparator, NodeCopier>::size() const {
  return size_;
}




};

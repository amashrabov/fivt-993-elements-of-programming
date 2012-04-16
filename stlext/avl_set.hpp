#include <memory>
#include "avl_tree_node.hpp"

namespace stlext {

template<class T>
class AvlSet {
 public:
  typedef typename AvlTreeNode<T>::node_ptr   node_ptr;
  typedef typename AvlTreeNode<T>::node_cptr  node_cptr;
  typedef typename AvlTreeNode<T>::node_sptr  node_sptr;
  typedef typename AvlTreeNode<T>::node_csptr node_csptr;

  AvlSet(): size_(0) {}
  bool insert(const T &value);
  bool remove(const T &value);
  bool contains(const T &value);

  size_t size() const;


 private:
  node_csptr root_;
  size_t size_;
};

template<class T>
bool AvlSet<T>::insert(const T& value) {
  if (root_.get() == NULL) {
    root_ = std::make_shared<const AvlTreeNode<T> >(value);
    ++size_;
    return true;
  }

  if (const_cast<node_ptr>(root_.get())->insert(root_, value)) {
    ++size_;
    return true;
  }
  
  return false;
}

template<class T>
bool AvlSet<T>::contains(const T& value) {
  if (root_.get() == NULL)
    return false;

  return const_cast<node_ptr>(root_.get())->contains(root_, value);
}

template<class T>
bool AvlSet<T>::remove(const T& value) {
  if (root_.get() == NULL)
    return false;

  bool ans = false;
  const_cast<node_ptr>(root_.get())->find_and_remove(root_, value, ans);
  
  if (ans)
    --size_;

  return ans;
}

template<class T>
size_t AvlSet<T>::size() const {
  return size_;
}

};

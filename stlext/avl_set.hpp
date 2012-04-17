#include <memory>
#include <functional>
#include "avl_tree_node.hpp"

namespace stlext {

template<class T, class Comparator = std::less<T> >
class AvlSet {
 public:
  typedef typename AvlTreeNode<T, Comparator>::node_ptr   node_ptr;
  typedef typename AvlTreeNode<T, Comparator>::node_cptr  node_cptr;
  typedef typename AvlTreeNode<T, Comparator>::node_sptr  node_sptr;
  typedef typename AvlTreeNode<T, Comparator>::node_csptr node_csptr;

  AvlSet(): size_(0) {}
  bool insert(const T &value);
  bool remove(const T &value);
  bool contains(const T &value) const;

  size_t size() const;

 private:
  node_csptr root_;
  size_t size_;
};

template<class T, class Comparator>
bool AvlSet<T, Comparator>::insert(const T& value) {
  if (root_.get() == NULL) {
    root_ = std::make_shared<const AvlTreeNode<T, Comparator> >(value);
    ++size_;
    return true;
  }

  if (const_cast<node_ptr>(root_.get())->insert(root_, value)) {
    ++size_;
    return true;
  }
  
  return false;
}

template<class T, class Comparator>
bool AvlSet<T, Comparator>::contains(const T& value) const {
  if (root_.get() == NULL)
    return false;

  return root_->contains(root_, value);
}

template<class T, class Comparator>
bool AvlSet<T, Comparator>::remove(const T& value) {
  if (root_.get() == NULL)
    return false;

  bool ans = false;
  const_cast<node_ptr>(root_.get())->find_and_remove(root_, value, ans);
  
  if (ans)
    --size_;

  return ans;
}

template<class T, class Comparator>
size_t AvlSet<T, Comparator>::size() const {
  return size_;
}

};

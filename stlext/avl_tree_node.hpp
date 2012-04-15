#include <memory>

namespace stlext {

template<class T>
class AvlTreeNode {
 public:
  typedef AvlTreeNode<T>* node_ptr;
  typedef AvlTreeNode<T>* const node_cptr;
  typedef std::shared_ptr<AvlTreeNode<T> > node_sptr;
  typedef std::shared_ptr<const AvlTreeNode<T> > node_csptr;

  template <class ... Args>
  AvlTreeNode(Args... args):
    value_(std::move(std::make_shared<T>(args...))),
    height_(1) {}

  node_csptr& find(node_csptr &ref, const T &value);
  bool contains(node_csptr &ref, const T &value);
  bool insert(node_csptr &ref, const T &value);
  void find_and_remove(node_csptr &ref, const T &value);  

 private:
  // Const functions
  int get_height(node_csptr &node) const;
  int get_factor(node_csptr &node) const;

  // Balance related functions
  void update_height(node_csptr &ref);  
  void balance(node_csptr &ref, bool inverse = false);
  void small_rotation(node_csptr &ref, bool inverse = false);
  void big_rotation(node_csptr &ref, bool inverse = false);

  // Removal related functions
  void remove_rec(node_csptr &ref, T &val, int dir);

  // DEbug related functions
  void invariant_check() const;
  
  int height_;
  T value_;
  node_csptr child_[2];
};

};

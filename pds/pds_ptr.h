#ifndef PDS_PTR_H_
#define PDS_PTR_H_

#include <memory>

namespace pds {

template <class Node>
class pds_ptr: public std::shared_ptr<const Node> {

  public:

  pds_ptr() : base() {
  }

  pds_ptr(Node* node_ptr) : base(node_ptr) {
  }

  Node* switch_to_mutable() {
    if (!this->get()) return nullptr;
    if (this->unique()) {
      return const_cast<Node*>((this->get()));
    } else {
      Node* my_clone(new Node(**this));
      this->base::operator=(static_cast<base>(my_clone));
      return my_clone;
    }
  }

  void operator=(Node* node_ptr) {
    this->base::operator=(static_cast<base>(node_ptr));
  }


  void operator=(std::unique_ptr<Node> node_ptr) {
    this->base::operator=(static_cast<base>(node_ptr));
  }

  private:

  typedef std::shared_ptr<const Node> base;

};

} // namespace pds

#endif /* PDS_PTR_H_ */

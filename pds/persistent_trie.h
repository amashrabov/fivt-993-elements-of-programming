// Copyright 2013 Artem Volkhin
#ifndef PDS_PERSISTENT_TRIE_H_
#define PDS_PERSISTENT_TRIE_H_

#include <map>
#include <memory>
#include <vector>

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstdio>

#include "pds/pds_ptr.h"

namespace pds {
template <class C>
class Trie {
    public:
        Trie() {
            root_ = new Node();
        }

        void add_word(const std::vector<C>& s) {
            node_ptr cur = root_.switch_to_mutable();
            for (auto ch : s) {
                cur = add_symbol(cur, ch);
            }
            cur->set_final();
        }

        bool contains(const std::vector<C>& s) {
            const_node_ptr cur = root_;
            for (auto ch : s) {
                if (!cur->has_child(ch))
                    return false;
                cur = cur->get_child(ch);
            }
            return cur->is_final();
        }


    private:
        class Node;
        typedef Node* node_ptr;
        typedef pds_ptr<Node> const_node_ptr;

        class Node {
            public:
                void set_final() {
                    is_final_ = true;
                }

                void unset_final() {
                    is_final_ = false;
                }

                bool is_final() const {
                    return is_final_;
                }

                const_node_ptr get_child(const C& name) const {
                    return childs_.at(name);
                }

                bool has_child(const C& name) const {
                    return childs_.count(name) > 0;
                }

                void set_child(const C& name, const_node_ptr value) {
                    childs_[name] = value;
                }

            private:
                bool is_final_;
                std::map<C, const_node_ptr> childs_;
        };

        const_node_ptr root_;

        node_ptr add_symbol(node_ptr cur, const C& ch) {
            node_ptr next = NULL;
            if (cur->has_child(ch)) {
                const_node_ptr const_next = cur->get_child(ch);
                next = const_next.switch_to_mutable();
                cur->set_child(ch, const_next);
            } else {
                next = new Node();
                const_node_ptr const_next(next);
                cur->set_child(ch, const_next);
            }
            return next;
        }
};

}  // namespace stlext

#endif  // PDS_PERSISTENT_TRIE_H_

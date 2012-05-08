#ifndef STLEXT_TRIE_H_
#define STLEXT_TRIE_H_

// TODO: check codestyle

#include <map>
#include <memory>

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstdio>

#include "pds_ptr.h"

#define DEBUG 1
#define debug_print(fmt, ...) \
    do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

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
                dump_trie();
                debug_print("%c %p\n", ch, cur);
                cur = add_symbol(cur, ch);
            }
            cur->set_final();
        }

        bool contains(const std::vector<C>& s) {
            const Node* cur = root_.get();
            for (auto ch : s) {
                if (!cur->has_child(ch))
                    return false;
                cur = cur->get_child(ch).get();
            }
            return cur->is_final();
        }


    private:
        class Node;
        typedef Node* node_ptr;
        typedef pds_ptr<Node> const_node_ptr;

        class Node {
            public:
                Node() : is_final_(false) {}

                explicit Node(const Node& o):
                    is_final_(o.is_final_), childs_(o.childs_) {}

                void set_final() {
                    is_final_ = true;
                }

                void unset_final() {
                    is_final_ = false;
                }

                bool is_final() const {
                    return is_final_;
                }

                const_node_ptr get_child(C name) const {
                    return childs_.at(name);
                }

                bool has_child(C name) const {
                    return childs_.count(name) > 0;
                }

                void set_child(C name, const_node_ptr value) {
                    childs_[name] = value;
                }
                
                void set_child(C name, node_ptr value) {
                    childs_[name] = value;
                }

                void dump_recursive(int offset = 0) const {
                    fprintf(stderr, "%p ", this);
                    for (int i = 0; i < offset; i++)
                        fprintf(stderr, " ");
                    fprintf(stderr, "childs&: %p ", &childs_);
                    for (auto ch : childs_) {
                        // fprintf(stderr, "%c %p; ", ch.first, ch.second.get());
                    }
                    fprintf(stderr, "\n");
                    for (auto ch : childs_)
                        ch.second->dump_recursive(offset + 1);
                }

            private:
                bool is_final_;
                std::map<C, const_node_ptr> childs_;
        };

        const_node_ptr root_;

        node_ptr add_symbol(node_ptr cur, C ch) {
            node_ptr new_child;
            if (cur->has_child(ch)) {
                new_child = cur->get_child(ch).switch_to_mutable();
                fprintf(stderr, "here\n");
            } else {
                new_child = new Node();
            }
            cur->set_child(ch, new_child);
            debug_print("%p\n", new_child);
            return new_child;
        }

        void dump_trie() {
            fprintf(stderr, "===BEGIN===\n");
            root_->dump_recursive();
            fprintf(stderr, "===END===\n");
        }
};

}  // namespace stlext

#endif  // STLEXT_TRIE_H_

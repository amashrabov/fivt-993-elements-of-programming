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

#define DEBUG 0
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
            const_node_ptr cur = root_;
            for (auto ch : s) {
                debug_print("\n\n", 0);
                dump_trie();
                debug_print("adding %c %p\n", ch, cur.get());
                cur = add_symbol(cur, ch);
            }
            // cur->set_final();
        }

        // bool contains(const std::vector<C>& s) {
            // const Node* cur = root_.get();
            // for (auto ch : s) {
                // if (!cur->has_child(ch))
                    // return false;
                // cur = cur->get_child(ch).get();
            // }
            // return cur->is_final();
        // }


    private:
        class Node;
        typedef Node* node_ptr;
        typedef pds_ptr<Node> const_node_ptr;

        class Node {
            public:
                Node() : is_final_(false), childs_() {
                    debug_print("*** Node(), %p\n", this);
                }

                explicit Node(const Node& o):
                    is_final_(o.is_final_), childs_(o.childs_) {
                    debug_print("*** Node(const Node& o), %p; this: %p\n", &o, this);
                    debug_print("####\n", 0);
                    this->dump_recursive();
                    debug_print("####\n", 0);
                }

                Node& operator= (const Node& o) {
                    debug_print("*** Node operator= %p; this: %p\n", &o, this);
                }

                ~Node() {
                    debug_print("*** ~Node(), %p\n", this);
                }

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

                // void set_child(C name, const const_node_ptr& value) {
                    // childs_[name] = value;
                // }
                
                void set_child(C name, const_node_ptr value) {
                    childs_[name] = value;
                }

                void dump_recursive(int offset = 0) const {
                    fprintf(stderr, "%p ", this);
                    for (int i = 0; i < offset; i++)
                        fprintf(stderr, " ");
                    fprintf(stderr, "childs&: %p ", &childs_);
                    for (auto ch : childs_) {
                        fprintf(stderr, "%c %p; ", ch.first, ch.second.get());
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

        const_node_ptr add_symbol(const_node_ptr cur, C ch) {
            const_node_ptr next;
            if (cur->has_child(ch)) {
                next = cur->get_child(ch);
                fprintf(stderr, "here\n");
            } else {
                next = new Node();
            }
            cur.switch_to_mutable()->set_child(ch, next);
            debug_print("next: %p\n", next.get());
            return next;
        }

        void dump_trie() {
            fprintf(stderr, "===BEGIN===\n");
            root_->dump_recursive();
            fprintf(stderr, "===END===\n");
        }
};

}  // namespace stlext

#endif  // STLEXT_TRIE_H_

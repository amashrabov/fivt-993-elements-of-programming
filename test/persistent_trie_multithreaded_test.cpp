#include "persistent_trie.h"

#include <gtest/gtest.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <iterator>
#include <string>
#include <thread>

namespace pds {
    std::vector<char> string2vector(const std::string& s);

    std::string gen_string() {
        int len = rand() % 100 + 1;
        std::string s = "";
        for (int i = 0; i < len; i++) {
            s += (char)(rand() % 26 + 'a');
        }
        return s;
    }


    class trie_checker {
        public:
        trie_checker(Trie<char> trie) : trie_(trie) {
        }

        void operator()(int count) {
            for (int i = 0; i < count; i++) {
                std::string s = gen_string();
                insert(s);
            }
        }

        void insert(std::string str) {
            archive_.push_back(string2vector(str));
            trie_.add_word(string2vector(str));
        }

        bool check() {
            for (auto word : archive_) {
                if (!trie_.contains(word))
                    return false;
            }
            return true;
        }

        private:
            Trie<char> trie_;
            std::vector<std::vector<char>> archive_;
    };

    TEST(persistent_trie_multithreaded, simple) {
        Trie<char> trie;
        int len = 1000;
        for (int i = 0; i < len; i++)
            trie.add_word(string2vector(gen_string()));
        trie_checker checker1(trie);
        trie_checker checker2(trie);
        std::thread thread1(checker1, 10000);
        std::thread thread2(checker2, 10000);
        thread1.join();
        thread2.join();
        ASSERT_EQ(checker1.check(), true);
        ASSERT_EQ(checker2.check(), true);
    }
}

#include <gtest/gtest.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <iterator>
#include <string>

#include "persistent_trie.h"

namespace pds {
    std::vector<char> string2vector(const std::string& s) {
        std::vector<char> res;
        std::copy(s.begin(), s.end(), std::back_inserter(res));
        return res;
    }

    TEST(persistent_trie, simple) {
        Trie<char> trie;
        trie.add_word(string2vector("abcde"));
        trie.add_word(string2vector("xyz"));
        trie.add_word(string2vector("abcer"));
        ASSERT_EQ(trie.contains(string2vector("abcde")), true);
        ASSERT_EQ(trie.contains(string2vector("ab")), false);
        trie.add_word(string2vector("ab"));
        ASSERT_EQ(trie.contains(string2vector("ab")), true);
        ASSERT_EQ(trie.contains(string2vector("xyz")), true);
        ASSERT_EQ(trie.contains(string2vector("abcde")), true);
    }

    TEST(persistent_trie, save_version) {
        Trie<char> trie;
        trie.add_word(string2vector("abcde"));
        ASSERT_EQ(trie.contains(string2vector("abcde")), true);
        ASSERT_EQ(trie.contains(string2vector("ab")), false);
        ASSERT_EQ(trie.contains(string2vector("xyz")), false);
        Trie<char> trie2(trie);
        trie2.add_word(string2vector("xyz"));
        ASSERT_EQ(trie.contains(string2vector("abcde")), true);
        ASSERT_EQ(trie.contains(string2vector("ab")), false);
        ASSERT_EQ(trie.contains(string2vector("xyz")), false);
        ASSERT_EQ(trie2.contains(string2vector("abcde")), true);
        ASSERT_EQ(trie2.contains(string2vector("ab")), false);
        ASSERT_EQ(trie2.contains(string2vector("xyz")), true);
    }

    TEST(persistent_trie, different_key_type) {
        Trie<std::string> trie;
        std::vector<std::string> v1;
        v1.push_back("abc");
        v1.push_back("xyz");
        v1.push_back("xyz");
        ASSERT_EQ(trie.contains(v1), false);
        trie.add_word(v1);
        trie.add_word(v1);
        ASSERT_EQ(trie.contains(v1), true);
    }
}

#include <gtest/gtest.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <iterator>

#include "persistent_trie.h"

namespace pds {
    std::vector<char> string2vector(const std::string& s) {
        std::vector<char> res;
        std::copy(s.begin(), s.end(), std::back_inserter(res));
        return res;
    }

    TEST(persistent_trie, simple) {
        Trie<char> trie;
        trie.add_word(string2vector("ab"));
        // trie.add_word(string2vector("xyz"));
        // ASSERT_EQ(trie.contains(string2vector("abcde")), true);
        // ASSERT_EQ(trie.contains(string2vector("ab")), false);
        trie.add_word(string2vector("ab"));
        // ASSERT_EQ(trie.contains(string2vector("ab")), false);
        // ASSERT_EQ(trie.contains(string2vector("abcde")), true);
        // ASSERT_EQ(trie.contains(string2vector("ab")), false);
    }
}

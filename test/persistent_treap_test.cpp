#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
#include <time.h>
#include <string>
#include <sstream>
#include "persistent_treap.h"
#include <gtest/gtest.h>


std::string inttostr(int number)
{
   std::stringstream ss;
   ss << number;
   return ss.str();
}


int get_rand(){
		if (RAND_MAX >= 32768) return rand();
		return (rand() << 15) ^ (rand());
	}

TEST(persistent_treap, access_by_order_and_persistence){
	int T_SIZE = 100;
	int T_NUMBER = 10;
	srand(time(NULL));
	std::vector<int> a[T_NUMBER];
	persistent_treap <int> t[T_NUMBER];
	t[0].clear();
	for (int test = 0; test < T_NUMBER; test++){
		if (test > 0) {
			t[test] = persistent_treap<int>(t[test-1]);
			a[test] = a[test-1];
		}
		for (int i = 1; i <= T_SIZE; ++i){
			int k = get_rand();
			a[test].push_back(k);
			if (!t[test].push(k)) a[test].pop_back();	
		}

//		std::sort(a[test].begin(), a[test].end());
		ASSERT_EQ(t[test].size(), a[test].size());		
	}

	for (int test = 0; test < T_NUMBER; test++) std::sort(a[test].begin(), a[test].end());

	for (int test = 0; test < T_NUMBER; test++){
		for (int i = 1; i <= a[test].size(); ++i){
			int k = t[test].get_kth(i);
			ASSERT_EQ(k, a[test][i-1]);
		}
	}
}

TEST(persistent_treap, performance_test){
	int T_SIZE = 1000000;
	int T_NUMBER = 1;
	std::vector<int> a;
	persistent_treap <int> t;
	for (int test = 0; test < T_NUMBER; test++){
		a = std::vector<int> (0,0);
		t.clear();
		for (int i = 1; i <= T_SIZE; ++i){
			int k = get_rand();
			a.push_back(k);
			if (!t.push(k)) a.pop_back();	
		}

		std::sort(a.begin(), a.end());
		ASSERT_EQ(t.size(), a.size());
		for (int i = 1; i <= a.size(); ++i){
			int k = t.get_kth(1);
			ASSERT_EQ(k, a[i-1]);
			t.pop(k);
		}
	}
}


TEST(persistent_treap, int_sort){
	int T_SIZE = 100;
	int T_NUMBER = 100;
	std::vector<int> a;
	persistent_treap <int> t;
	for (int test = 0; test < T_NUMBER; test++){
		a = std::vector<int> (0,0);
		t.clear();
		for (int i = 1; i <= T_SIZE; ++i){
			int k = get_rand();
			a.push_back(k);
			if (!t.push(k)) a.pop_back();	
		}

		std::sort(a.begin(), a.end());
		ASSERT_EQ(t.size(), a.size());
		for (int i = 1; i <= a.size(); ++i){
			int k = t.get_kth(1);
			ASSERT_EQ(k, a[i-1]);
			t.pop(k);
		}
	}
}

TEST(persistent_treap, string_sort){
	int T_SIZE = 100;
	int T_NUMBER = 100;
	std::vector<std::string> a;
	persistent_treap <std::string> t;
	for (int test = 0; test < T_NUMBER; test++){
		a = std::vector<std::string> (0);
		t.clear();
		for (int i = 1; i <= T_SIZE; ++i){
			std::string k = inttostr(get_rand());
			a.push_back(k);
			if (!t.push(k)) a.pop_back();	
		}

		std::sort(a.begin(), a.end());
		ASSERT_EQ(t.size(), a.size());
		for (int i = 1; i <= a.size(); ++i){
			std::string k = t.get_kth(1);
			ASSERT_EQ(k, a[i-1]);
			t.pop(k);
		}
	}
}


TEST(persistent_treap, random_access){
	int T_SIZE = 100;
	int T_NUMBER = 100;
	std::vector<int> a;
	persistent_treap <int> t;
	for (int test = 0; test < T_NUMBER; test++){
		a = std::vector<int> (0,0);
		t.clear();
		for (int i = 1; i <= T_SIZE; ++i){
			int k = get_rand();
			a.push_back(k);
			if (!t.push(k)) a.pop_back();	
		}

		std::sort(a.begin(), a.end());
		ASSERT_EQ(t.size(), a.size());
		for (int i = 1; i <= a.size(); ++i){
			int rand_ind = get_rand() % a.size();
			int k = t.get_kth(rand_ind+1);
			ASSERT_EQ(k, a[rand_ind]);
		}
	}
}







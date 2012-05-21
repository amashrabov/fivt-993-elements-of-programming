#include <gtest/gtest.h>

#include <string>
#include <stack>
#include <assert.h>
#include "persistent_stack.h"

namespace pds{

TEST(persistent_stack_int, simple){
	persistent_stack<int> s1;
	for (int i = 0; i < 10; i++)
		s1.push(i);
	s1.push(10);
	for (int i = 10; i >=0; i --) {
		int t = s1.top();
		s1.pop();
		 ASSERT_EQ(t, i);
	}
	ASSERT_TRUE(s1.empty());
}

TEST(persistent_stack_int, two_stacks_1){
	persistent_stack<int> s1;
	for (int i = 0; i < 10; i++)
		s1.push(i);
	persistent_stack<int> s2 = s1;
	s2.push(11);
	s1.push(10);
	for (int i = 10; i >=0; i --) {
		int t = s1.top();
		s1.pop();
		ASSERT_EQ(t, i);
	}
	for (int i = 11; i >=0; i --) {
		if (i == 10) continue;
		int t = s2.top();
		s2.pop();
		ASSERT_EQ(t, i);
	}
	ASSERT_TRUE(s1.empty());
	ASSERT_TRUE(s2.empty());
}

TEST(persistent_stack_int, two_stacks_2){
	persistent_stack<int> s1;
	s1.push(2);
	s1.push(3);
	persistent_stack<int> s3 = s1;
	s1.push(4);
	for (int i = 4; i >= 2; i --) {
		int t = s1.top();
		s1.pop();
		ASSERT_EQ(i, t);
	}
	for (int i = 3; i >= 2; i --) {
		int t = s3.top();
		s3.pop();
		ASSERT_EQ(i, t);
	}
	ASSERT_TRUE(s1.empty());
	ASSERT_TRUE(s3.empty());
}

TEST(persistent_stask_string, two_stacks){
	persistent_stack<std::string> t1;
	persistent_stack<std::string> t2 = t1;
	for (int i = 0; i < 10; i++)
		t1.push("sdf");
	for (int i = 0; i < 10; i++)
		t2.push("qwe");
	ASSERT_FALSE(t1.empty());
	ASSERT_FALSE(t2.empty());
	for (int i = 0; i < 10; i++) {
		std::string t = t1.top();
		t1.pop();
		ASSERT_EQ(t, "sdf");
	}
	ASSERT_TRUE(t1.empty());
	for (int i = 0; i < 10; i++) {
		std::string t = t2.top();
		t2.pop();
		ASSERT_EQ(t, "qwe");
	}
	ASSERT_TRUE(t2.empty());
}

TEST(pesistent_stack_int, huge){
	std::stack<int>* std_s = new std::stack<int>;
	persistent_stack<int> per_s;
	for (int i = 0; i <= 10000000; i++){
		int r = rand();
		std_s->push(r);
		per_s.push(r);
	}
	int fails = 0; 

	for (int i = 0; i <= 10000000; i++){
		int r = std_s->top();
		std_s->pop();
		if (r != per_s.top()) fails ++;
		per_s.pop();
	}
	delete std_s;
	ASSERT_EQ(fails, 0);
}

}

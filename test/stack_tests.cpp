#include <iostream>
#include <string>
#include <stack>
#include "persistent_stack.h"

using std::cin;
using std::cout;
using std::endl;
using std::stack;
using std::string;
using namespace psd;

int main(){
	persistent_stack<int> s1;
	for (int i = 0; i < 10; i++)
		s1.push(i);
	persistent_stack<int> s2 = s1;
	s2.push(11);
	s1.push(10);
	cout << "Expected numbers 10 - 0" << endl;
	while (!s1.empty()) {
		int t = s1.top();
		s1.pop();
		cout << t  << endl;
	}
	cout << "Expected 11 and 9 - 0" << endl;
	while (!s2.empty()) {
		int t = s2.top();
		s2.pop();
		cout << t  << endl;
	}
	s1.push(2);
	s1.push(3);
	persistent_stack<int> s3 = s1;
	s1.push(4);
	cout << "Expected 4 - 2" << endl;
	while (!s1.empty()) {
		int t = s1.top();
		s1.pop();
		cout << t  << endl;
	}
	cout << "Expected 3, 2" << endl;
	while (!s3.empty()) {
		int t = s3.top();
		s3.pop();
		cout << t  << endl;
	}
	
	persistent_stack<string> t1;
	persistent_stack<string> t2;
	
	for (int i = 0; i < 10; i++)
		t1.push("sdf");
	for (int i = 0; i < 10; i++)
		t2.push("qwe");
	cout << "Expected 10 sdf's" << endl;
	while (!t1.empty()) {
		string t = t1.top();
		t1.pop();
		cout << t  << endl;
	}
	cout << "Expected 10 qwe's" << endl;
	while (!t2.empty()) {
		string t = t2.top();
		t2.pop();
		cout << t  << endl;
	}
	if (true){
	cout << "Stress test: pushing 10 00 000 ints into this stack and the standard one" << endl;
	stack<int>* std_s = new stack<int>;
	persistent_stack<int> per_s;
	for (int i = 0; i <= 10000000; i++){
		int r = rand();
		if (i % 1000000 == 0) cout << i/ 100000 << "%" << endl;
		std_s->push(r);
		per_s.push(r);
	}
	int fails = 0; 
	cout << "Comparing" << endl;

	for (int i = 0; i <= 10000000; i++){
		int r = std_s->top();
		if (i % 1000000 == 0) cout << i/ 100000 << "%" << endl;
		std_s->pop();
		if (r != per_s.top()) fails ++;
		per_s.pop();
	}
	cout << "Deleting the standard stack" << endl;
	delete std_s;
	cout << "Finished with " << fails << " fails" << endl;
	}
	cout << "Cathing empty error" << endl;
	try {
		s1.pop();
	}
	catch (empty){
		cout << "Cought" << endl;
	}
	
	
	char a; cin >> a;
	return 0;
}

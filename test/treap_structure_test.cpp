//#include "treap.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
#include <time.h>
#include <string>
#include <sstream>
#include "persistent_treap.h"

const int num = 100;
const int TREAP_SIZE = 200;
int count = 0;


std::string inttostr(int number)
{
   std::stringstream ss;
   ss << number;
   return ss.str();
}


void check_order(persistent_treap<std::string> & t){
	for (int i = 1; i < t.size(); i++){
		if (t.get_kth(i+1) <= t.get_kth(i)){
			std::cout << std::endl << "error" << t.get_kth(i) << " " << t.get_kth(i+1) << std::endl;
			return;
		}
	}
	std::cout << std::endl << "nice done" << std::endl;
}

void check_pop(persistent_treap<int> & t, int tr){
	for (int i = 1; i < TREAP_SIZE; ++i){
		t.push(i);
	}
	for (int i = 1; i < t.size(); ++i){
		int k = t.get_kth(1);
		//t.print_tree();
		//std::cout<<"###################################\n";
		if (k!= i){ 
			std::cout<<"error i = " << i << "; try = " << tr << std::endl;			
			exit(100);
		}
		t.pop(k);
	}
}

void check_sort(persistent_treap<int> & t){
	std::vector<int> a;
	t.clear();
	for (int i = 1; i <= TREAP_SIZE; ++i){
		int k = rand()%100000;
		a.push_back(k);
		if (!t.push(k)) a.pop_back();		
	}

	std::sort(a.begin(), a.end());
	if (a.size() != t.size()) {
		std::cout << "size error";
		t.print_tree();
		exit(100);
	}
	for (int i = 1; i <= a.size(); ++i){
		int k = t.get_kth(1);
		if (k != a[i-1]) {
			std::cout << "sort fail " << k <<  std::endl;
			t.print_tree();
			exit(100);
		}
		t.pop(k);
	}
}

void check_sort_string(persistent_treap<std::string> & t){
	std::vector<std::string> a;
	t.clear();
	for (int i = 1; i <= TREAP_SIZE; ++i){
		std::string k = inttostr(rand()%100000);
		a.push_back(k);
		if (!t.push(k)) a.pop_back();	
	}

	std::sort(a.begin(), a.end());
	if (a.size() != t.size()) {
		std::cout << "size string error";
		exit(100);
	}
	for (int i = 1; i <= a.size(); ++i){
		std::string k = t.get_kth(1);
		if (k != a[i-1]) {
			std::cout << "sort string fail " << k <<  std::endl;
			exit(100);
		}
		t.pop(k);
	}
}





int main(){

	
	srand(time(NULL));

	
	int done = 0;	

	persistent_treap<std::string> ts[num];
	for (int i = 0; i < num; i++){
		check_sort_string(ts[i]);
		done = 100*(i+1)/num;
		std::cout<< done << "%" << std::endl;
	}
	
	std::cout<< "almost done" << std::endl;

	persistent_treap<int> t[num];
	for (int i = 0; i < num-1; i++){
	check_pop(t[i], i);
	t[i].clear();	
	t[i+1] = persistent_treap<int> (t[i]);


	}

	for (int i = 0; i < num; i++){
		check_sort(t[i]);
		done = 100*(i+1)/num;
		std::cout<< done << "%" << std::endl;
	}
	
	freopen ("output.txt", "w", stdout);	
	std::cout << "ok";
	



	return 0;
}

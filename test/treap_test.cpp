#include "treap.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
#include <time.h>
#include <string>
#include <sstream>

const int num = 99;



std::string inttostr(int number)
{
   std::stringstream ss;
   ss << number;
   return ss.str();
}


void check_order_string(const treap<std::string> t){

}

void fill_with_rand_int(treap<int> & t){
	for (int i = 0; i < num; i++){
		int k = rand()%100;
		t.insert(k);
	}
}

void fill_with_int(treap<int> &t){
	for (int i = 1; i < 100; i++){
		t.insert(i);
		t.insert(i);
	}
}

void fill_with_string(treap<std::string> & t){
	for (int i = 1; i < 100; i++){
		t.insert(inttostr(i));
		t.insert(inttostr(i));
	}
}

void check_order(treap<std::string> & t){
	for (int i = 1; i < t.size(); i++){
		if (t.getkth(i+1) <= t.getkth(i)){
			std::cout << std::endl << "error, bleyat'  " << t.getkth(i) << " " << t.getkth(i+1) << std::endl;
			return;
		}
	}
	std::cout << std::endl << "nice done" << std::endl;
}




int main(){
	treap<std::string> t;
	freopen ("output.txt", "w", stdout);
	srand(time(NULL));

	fill_with_string(t);
	check_order(t);


	//for (int i = 0; i < t.size(); std::cout << t.getkth(i+1) << " ", i++){	}
	

/*	t = treap<std::string>("a", "b", "c", "c");	

	for (int i = 0; i < t.size(); i++){
		std::cout << t.getkth(i+1) << " ";
	}
	*/
}
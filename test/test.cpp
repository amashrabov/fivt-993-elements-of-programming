#include "persistent_rope.h"
#include <ctime>
#include <cstdlib>
#include <iostream>

int main(){
	srand(time(NULL));
	pds::persistent_rope<int> rope;

	int N = 10000;

	for (int i = 0; i < N; i++){
		rope.push_back(i);
	}

	for (int i = 0; i < N; i++){
		std::cout << rope[i] << std::endl;
	}

	rope.reverse();

	std::cout << "After reversing" << std::endl;
	for (int i = 0; i < N; i++){
		std::cout << rope[i] << std::endl;
	}

	for (int i = 0; i < N; i++){
		rope.pop_back();
	}

	if (rope.empty()){
		std::cout << "ok" << std::endl;
	}else{
		std::cout << "there's a mistake =(" << std::endl;
	}

	return 0;
}

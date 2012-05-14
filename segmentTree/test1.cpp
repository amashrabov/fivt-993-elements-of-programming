#include "PersSegmTree.h"

#include <iostream>
#include <cstdio>
#include <memory>



int main()
{

	double x = 0;
	pds::PersistantSegmentTree<double> tree(10, x); // 0 0 0 0 0 0 0 0 0 0
	//std::cerr << "MADE\n";	
	//tree.writeTree(std::cout);

	tree.assign(2, 6, 1.2); // 0 0 1.2 1.2 1.2 1.2 1.2 0 0 0

	printf("%.10lf\n", tree.findSum(5, 7)); //2.4
	printf("%.10lf\n", tree.findSum(1, 1)); //0
	printf("%.10lf\n", tree.findSum(2, 5)); //4.8

	//tree.writeTree(std::cout);

	tree.add(4, 9, -2); // 0 0 1.2 1.2 -0.8 -0.8 -0.8 -2 -2 -2

	//tree.writeTree(std::cout);

	printf("%.10lf\n", tree.findSum(2, 5)); //0.8

	return 0;
}

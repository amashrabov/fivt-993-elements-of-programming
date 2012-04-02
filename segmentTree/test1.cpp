#include "PersSegmTreeML.h"

#include <iostream>
#include <cstdio>
#include <memory>



int main()
{

	double x;
	stlext::PersistantSegmentTree<double> tree(true, 10, x);
	tree.assign(2, 6, 1.2);

	printf("%.10lf\n", tree.findSum(5, 7));
	printf("%.10lf\n", tree.findSum(1, 1));
	printf("%.10lf\n", tree.findSum(2, 5));

	tree.add(4, 9, -2);

	printf("%.10lf\n", tree.findSum(2, 5));

	return 0;
}

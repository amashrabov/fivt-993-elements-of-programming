#include "PersSegmTree.h"
#include "WeakSTVersion.h"

#include <iostream>
#include <cstdio>
#include <memory>

struct Point
{
	int x, y, z;
	Point(){};	
	Point(int x, int y, int z)
	:x(x), y(y), z(z)
	{}
};
Point operator + (const Point &p1, const Point &p2)
{
	return Point(p1.x+p2.x, p1.y+p2.y, p1.z+p2.z);
}
Point operator * (size_t n, const Point &p1)
{
	return Point(n*p1.x, n*p1.y, n*p1.z);
}
std::ostream& operator << (std::ostream &fout, const Point &p)
{
	fout << "{" << p.x << " " << p.y << " " << p.z << "}";
	return fout;
}
int main()
{
	stlext::PersistantSegmentTree<Point> tree(true, 23, Point(0, 1, 2));
	stlext::PersistantSegmentTree<Point> tree1 = tree;
	tree.add(4, 10, Point(-2, -1, 0));
	tree1.assign(1, 12, Point(0, 0, 3));
	std::cout << tree.findSum(2, 7) << " " << tree1.findSum(2, 7) << "\n";
	//tree.writeTree(std::cout);
	//tree1.writeTree(std::cout);
	WeakSTVersion<Point> str(23, Point(0, 1, 2));
	WeakSTVersion<Point> str1 = str;
	str.add(4, 10, Point(-2, -1, 0));
	str1.assign(1, 12, Point(0, 0, 3));
	std::cout << str.findSum(2, 7) << " " << str1.findSum(2, 7) << "\n";
	return 0;
}

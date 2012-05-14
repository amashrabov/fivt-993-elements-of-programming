#ifndef _SEGMENT_TREE_TEST_H_
#define _SEGMENT_TREE_TEST_H_

#include "PersSegmTree.h"
#include "WeakSTVersion.h"
#include <iostream>
#include <fstream>
#include <functional>
#include <utility>
#include <algorithm>

using namespace pds;

struct Deg
{
	long long operator ()(size_t x, long long a)
	{
		long long ans = 1;
		while (x)
		{
			if (x % 2 == 1)
				ans *= a;
			a *= a;
			x /= 2;
		}
		return ans;
	}

};

std::pair<size_t, size_t> genRandomInterval(size_t size)
{
	size_t l = std::rand()%size;
	size_t r = std::rand()%size;
	if (l > r)
		std::swap(l, r);
	return std::make_pair(l, r);
}
const int STTESTASSIGN = 0;
const int STTESTADD = 1;
const int STTESTFINDSUM = 2;

void randomStress(std::ostream &fout, int size)
{ 
	fout << "=========================== RANDOM STRESS ==============================================\n";
	typedef PersistantSegmentTree<long long, std::multiplies<long long>, Deg> T1;
	typedef WeakSTVersion<long long, std::multiplies<long long> > T2;

	std::vector<T1> tree1(1, T1(size, (long long)0));
	std::vector<T2> tree2(1, T2(size, (long long)0));
	for (int vvv = 0; vvv <= size; vvv++)
	{
		int num = rand()%tree1.size();
		tree1.push_back(tree1[num]);
		tree2.push_back(tree2[num]);
		int operCode = rand()%3;
		std::pair<size_t, size_t> p = genRandomInterval(size);
		//fout << operCode << " " << p.first << " " << p.second << "\n";
		
		switch (operCode)
		{
			case STTESTASSIGN :
			{
				long long w = rand()%3;
				tree1.back().assign(p.first, p.second, w);
				tree2.back().assign(p.first, p.second, w);
		//		fout << "ASSIGN " << p.first << " " << p.second << "\n";
			}
			case STTESTADD :
			{
				long long w = rand()%3;
				tree1.back().add(p.first, p.second, w);
				tree2.back().add(p.first, p.second, w);
		//		fout << "ADD " << p.first << " " << p.second << "\n";
			}
			case STTESTFINDSUM :
			{
				long long w1 = tree1.back().findSum(p.first, p.second);
				long long w2 = tree2.back().findSum(p.first, p.second);
			//	fout << "FINDSUM " << p.first << " " << p.second << "\n";				
				if (w1 != w2)
				{
					fout << "ERROR: " << w1 << " " << w2 << "\n";
					return;
				}
		//		else
		//			fout << "OK " << w1 << "\n";	
			}			
		}
	}
	fout << "=========================== DONE OK ==============================================\n";
}
void randomStress2(std::ostream &fout, int size, int kol, int koloper)
{ 
	fout << "=========================== RANDOM STRESS 2 ==============================================\n";
	typedef PersistantSegmentTree<long long, std::multiplies<long long>, Deg> T1;
	typedef WeakSTVersion<long long, std::multiplies<long long> > T2;

	std::vector<T1> tree1(kol, T1(size, (long long)0));
	std::vector<T2> tree2(kol, T2(size, (long long)0));
	for (int vvv = 0; vvv <= koloper; vvv++)
	{
		int num = rand()%kol;
		int operCode = rand()%2;
		std::pair<size_t, size_t> p = genRandomInterval(size);
		//fout << operCode << " " << p.first << " " << p.second << "\n";
		
		switch (operCode)
		{
			case STTESTASSIGN :
			{
				long long w = rand()%3;
				tree1[num].assign(p.first, p.second, w);
				tree2[num].assign(p.first, p.second, w);
		//		fout << num << ": ASSIGN " << w << " on " << p.first << " " << p.second << "\n";
			}
			case STTESTADD :
			{
				long long w = rand()%3;
				tree1[num].add(p.first, p.second, w);
				tree2[num].add(p.first, p.second, w);
		//		fout << num << ": ADD " << w << " on " << p.first << " " << p.second << "\n";
			}
		}
		for (size_t i = 0; i < size; i++)
			for (size_t j = i; j < size; j++)
			{
				long long w1 = tree1[num].findSum(i, j);
				long long w2 = tree2[num].findSum(i, j);
		//		fout << "FINDSUM " << i << " " << j << "\n";				
				if (w1 != w2)
				{
					fout << "ERROR: " << w1 << " " << w2 << "\n";
					return;
				}
		//		else
		//			fout << "OK " << w1 << "\n";	
			}		
	}
	fout << "=========================== DONE OK ==============================================\n";
}
#endif

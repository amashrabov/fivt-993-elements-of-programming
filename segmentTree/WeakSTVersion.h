#ifndef _WEAK_VERSION_OF_SEGMENT_TREE_DPR-PAVLIN_
#define _WEAK_VERSION_OF_SEGMENT_TREE_DPR-PAVLIN_

#include <vector>

template <typename T, typename TSum = std::plus<T> >
class WeakSTVersion
{
private:
	TSum summator_;
	std::vector<T> a_;
public:
	WeakSTVersion(size_t size, const T &obj)
	{
		a_.assign(size, obj);
	}
	T findSum(size_t l, size_t r)
	{
		T ans = a_[l];
		for (size_t i = l+1; i <= r; i++)
			ans = summator_(ans, a_[i]);
		return ans;
	}	
	void assign(size_t l, size_t r, const T &obj)
	{
		for (size_t i = l; i <= r; i++)
			a_[i] = obj;
	}
	void add(size_t l, size_t r, const T &obj)
	{
		for (size_t i = l; i <= r; i++)
			a_[i] = summator_(a_[i], obj);
	}
};

#endif

#pragma once
#include <vector>
#include "ATICompare.h"

using std::vector;

template <class T>
class ATHeap
{
public:
	/**compare左小于右，则为true**/
	static void createBigHeap(vector<T>& raw, ATICompare<T>& compare)
	{
		int n = raw.size();
		//先建立大顶堆
		for(int i = n/2 - 1; i >= 0; --i)
		{
			heapAdjustImpl(raw, i, n - 1, compare);
		}
	}

	/**入堆，假定堆已是大顶堆**/
	static void heapIn(vector<T>& raw, T& value, ATICompare<T>& compare)
	{
		raw.push_back(value);
		if(raw.size() > 1)
		{
			T temp = raw[raw.size() - 1];
			raw[raw.size() - 1] = raw[0];
			raw[0] = temp;

			heapAdjust(raw, 0, raw.size() - 1, compare);
		}
	}

	/**出堆**/
	static T heapOut(vector<T>& raw, ATICompare<T>& compare)
	{
		if(raw.size() == 0)
		{
			throw "heap empty";
		}
		if(raw.size() == 1)
		{
			T result = raw[0];
			raw.pop_back();
			return result;
		}

		T result = raw[0];
		T temp = raw[0];
		raw[0] = raw[raw.size() - 1];
		raw[raw.size() - 1] = temp;

		raw.pop_back();
		heapAdjustImpl(raw, 0, raw.size() - 1, compare);

		return result;
	}

	/**堆调整**/
	static void heapAdjust(vector<T>& raw, int start, int end, ATICompare<T>& compare)
	{
		if(start < 0 || start > end || start > raw.size() - 1)
		{
			return;
		}

		heapAdjustImpl(raw, start, end, compare);
	}

private:
	/**调整堆**/
	static void heapAdjustImpl(vector<T>& raw, int start, int end, ATICompare<T>& compare)
	{
		T temp = raw[start];

		for(int i = 2 * start + 1; i <= end; i *= 2)
		{
			//因为假设根结点的序号为0而不是1，所以i结点左孩子和右孩子分别为2i+1和2i+2
			if(i < end && compare.compare(raw[i], raw[i+1]))//左右孩子的比较
			{
				++i;//i为较大的记录的下标
			}

			if(!compare.compare(temp, raw[i]))//左右孩子中获胜者与父亲的比较
			{
				break;
			}

			//将孩子结点上位，则以孩子结点的位置进行下一轮的筛选
			raw[start]= raw[i];
			start = i;
		}

		raw[start]= temp; //原始堆顶插入最终，最大的孩子位置
	}
};
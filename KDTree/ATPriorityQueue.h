#pragma once
#include "ATHeap.h"
#include "ATICompare.h"

template <class T>
class ATPriorityQueue
{
public:
	ATPriorityQueue(ATICompare<T>& cp, int cap = -1) : comp(cp), capicity(cap) {	}

	//出队，大顶堆，值大的出队
	T poll()
	{
		if(data.size() == 0)
		{
			throw "empty priority queue";
		}

		return ATHeap<T>::heapOut(data, comp);
	}

	//入队
	void add(T& value)
	{
		if(capicity == -1)
		{
			ATHeap<T>::heapIn(data, value, comp); //未设置capicity则堆大小无限制
		}
		else
		{
			if( data.size() < capicity)
			{
				ATHeap<T>::heapIn(data, value, comp);
			}
			else
			{
				/**当前值小于大根堆顶元素时，替换堆顶元素，并重新调整堆**/
				if(comp.compare(value, data[0]))
				{
					data[0] = value;
					ATHeap<T>::heapAdjust(data, 0, data.size() - 1, comp);
				}
			}
		}
	}

	vector<T>& allData()
	{
		return data;
	}

	int size()
	{
		return data.size();
	}

private:
	vector<T> data; //heap源数据
	ATICompare<T>& comp;
	int capicity;
};
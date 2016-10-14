#pragma once
#include "ATHeap.h"
#include "ATICompare.h"

template <class T>
class ATPriorityQueue
{
public:
	ATPriorityQueue(ATICompare<T>& cp, int cap = -1) : comp(cp), capicity(cap) {	}

	//���ӣ��󶥶ѣ�ֵ��ĳ���
	T poll()
	{
		if(data.size() == 0)
		{
			throw "empty priority queue";
		}

		return ATHeap<T>::heapOut(data, comp);
	}

	//���
	void add(T& value)
	{
		if(capicity == -1)
		{
			ATHeap<T>::heapIn(data, value, comp); //δ����capicity��Ѵ�С������
		}
		else
		{
			if( data.size() < capicity)
			{
				ATHeap<T>::heapIn(data, value, comp);
			}
			else
			{
				/**��ǰֵС�ڴ���Ѷ�Ԫ��ʱ���滻�Ѷ�Ԫ�أ������µ�����**/
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
	vector<T> data; //heapԴ����
	ATICompare<T>& comp;
	int capicity;
};
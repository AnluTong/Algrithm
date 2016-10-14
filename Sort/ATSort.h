#pragma once
#include <vector>
#include "ATICompare.h"
#include "ATHeap.h"

using std::vector;

template <class T>
class ATSort
{
public:
	/**��С��������compare��С���ң���Ϊtrue��ԭʼ���ݱ�����**/
	static void sort(vector<T>& raw, ATICompare<T>& compare)
	{
		if(raw.size() == 0 || raw.size() == 1)
		{
			return;
		}

		HeapSort(raw, compare);
	}

	/**ԭ���ݱ���������K�����ݴ�С��������������compare��С���ң���Ϊtrue**/
	static void getMaxK(vector<T>& raw, int k, ATICompare<T>& compare)
	{
		if(raw.size() < k || k == 0)
		{
			return;
		}

		HeapPartlySort(raw, k, compare);
	}

private:
	/**��������**/
	static void quickSort(vector<T>& raw, int low, int high, ATICompare<T>& compare)
	{
		if (low < high)
		{
			int l = low;
			int r = high;
			T key = vec[l];//��¼keyֵ

			while (l < r)
			{
				while (l < r && compare.compare(key, vec[r]))//�����������,�ҵ���һ��С��key��Ԫ��
					--r;
				vec[l] = vec[r];
				while (l < r 
					&& !compare.compare(key, vec[l]))//�������ұ���,�ҵ���һ������keyֵ��Ԫ��
					++l;
				vec[r] = vec[l];
			}
			vec[l] = key;//��ʵ��ʱl=r

			quicksort(vec, low, l-1, compare);
			quicksort(vec, r + 1, high, compare);
		}
	}

	/**������**/
	static void HeapSort(vector<T>& raw, ATICompare<T>& compare)
	{
		int n = raw.size();
		//�Ƚ����󶥶�
		for(int i = n/2 - 1; i >= 0; --i)
		{
			ATHeap<T>::heapAdjust(raw, i, n - 1, compare);
		}

		for(int i = n - 1; i >= 1; --i)  
		{  
			T temp = raw[0];   //����ǰ�ѵĸ��ڵ㽻������β��ָ��λ��  
			raw[0] = raw[i];  
			raw[i] = temp;  
  
			ATHeap<T>::heapAdjust(raw, 0, i - 1, compare);  //������һ�ε�����  
		}

	}

	/**���ֶ�����**/
	static void HeapPartlySort(vector<T>& raw, int k, ATICompare<T>& compare)
	{
		int n = raw.size();
		//�Ƚ����󶥶�
		for(int i = n/2 - 1; i >= 0; --i)
		{
			ATHeap<T>::heapAdjust(raw, i, n - 1, compare);
		}

		//��������
		int limit = (n - k - 1 < 0) ? 0 : n - k - 1;
		for(int i = n - 1; i > limit; --i)
		{
			//���һ��Ԫ�غ͵�һԪ�ؽ��н���
			T temp = raw[i];
			raw[i] = raw[0];
			raw[0] = temp;

			//Ȼ��ʣ�µ�����Ԫ�ؼ�������Ϊ�󶥶�
			ATHeap<T>::heapAdjust(raw, 0, i - 1, compare);
		}
	}
};
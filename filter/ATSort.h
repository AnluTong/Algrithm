#pragma once
#include <vector>
#include "ATICompare.h"
#include "ATHeap.h"

using std::vector;

template <class T>
class ATSort
{
public:
	/**从小到大排序，compare左小于右，则为true，原始数据被排序**/
	static void sort(vector<T>& raw, ATICompare<T>& compare)
	{
		if(raw.size() == 0 || raw.size() == 1)
		{
			return;
		}

		HeapSort(raw, compare);
	}

	/**原数据被部分排序，K个数据从小到大放在数组最后，compare左小于右，则为true**/
	static void getMaxK(vector<T>& raw, int k, ATICompare<T>& compare)
	{
		if(raw.size() < k || k == 0)
		{
			return;
		}

		HeapPartlySort(raw, k, compare);
	}

private:
	/**快速排序**/
	static void quickSort(vector<T>& raw, int low, int high, ATICompare<T>& compare)
	{
		if (low < high)
		{
			int l = low;
			int r = high;
			T key = vec[l];//记录key值

			while (l < r)
			{
				while (l < r && compare.compare(key, vec[r]))//从右往左遍历,找到第一个小于key的元素
					--r;
				vec[l] = vec[r];
				while (l < r 
					&& !compare.compare(key, vec[l]))//从左往右遍历,找到第一个大于key值的元素
					++l;
				vec[r] = vec[l];
			}
			vec[l] = key;//其实此时l=r

			quicksort(vec, low, l-1, compare);
			quicksort(vec, r + 1, high, compare);
		}
	}

	/**堆排序**/
	static void HeapSort(vector<T>& raw, ATICompare<T>& compare)
	{
		int n = raw.size();
		//先建立大顶堆
		for(int i = n/2 - 1; i >= 0; --i)
		{
			ATHeap<T>::heapAdjust(raw, i, n - 1, compare);
		}

		for(int i = n - 1; i >= 1; --i)  
		{  
			T temp = raw[0];   //将当前堆的根节点交换到堆尾的指定位置  
			raw[0] = raw[i];  
			raw[i] = temp;  
  
			ATHeap<T>::heapAdjust(raw, 0, i - 1, compare);  //建立下一次的最大堆  
		}

	}

	/**部分堆排序**/
	static void HeapPartlySort(vector<T>& raw, int k, ATICompare<T>& compare)
	{
		int n = raw.size();
		//先建立大顶堆
		for(int i = n/2 - 1; i >= 0; --i)
		{
			ATHeap<T>::heapAdjust(raw, i, n - 1, compare);
		}

		//进行排序
		int limit = (n - k - 1 < 0) ? 0 : n - k - 1;
		for(int i = n - 1; i > limit; --i)
		{
			//最后一个元素和第一元素进行交换
			T temp = raw[i];
			raw[i] = raw[0];
			raw[0] = temp;

			//然后将剩下的无序元素继续调整为大顶堆
			ATHeap<T>::heapAdjust(raw, 0, i - 1, compare);
		}
	}
};
#pragma once
#include <vector>
#include "ATICompare.h"

using std::vector;

template <class T>
class ATHeap
{
public:
	/**compare��С���ң���Ϊtrue**/
	static void createBigHeap(vector<T>& raw, ATICompare<T>& compare)
	{
		int n = raw.size();
		//�Ƚ����󶥶�
		for(int i = n/2 - 1; i >= 0; --i)
		{
			heapAdjustImpl(raw, i, n - 1, compare);
		}
	}

	/**��ѣ��ٶ������Ǵ󶥶�**/
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

	/**����**/
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

	/**�ѵ���**/
	static void heapAdjust(vector<T>& raw, int start, int end, ATICompare<T>& compare)
	{
		if(start < 0 || start > end || start > raw.size() - 1)
		{
			return;
		}

		heapAdjustImpl(raw, start, end, compare);
	}

private:
	/**������**/
	static void heapAdjustImpl(vector<T>& raw, int start, int end, ATICompare<T>& compare)
	{
		T temp = raw[start];

		for(int i = 2 * start + 1; i <= end; i *= 2)
		{
			//��Ϊ������������Ϊ0������1������i������Ӻ��Һ��ӷֱ�Ϊ2i+1��2i+2
			if(i < end && compare.compare(raw[i], raw[i+1]))//���Һ��ӵıȽ�
			{
				++i;//iΪ�ϴ�ļ�¼���±�
			}

			if(!compare.compare(temp, raw[i]))//���Һ����л�ʤ���븸�׵ıȽ�
			{
				break;
			}

			//�����ӽ����λ�����Ժ��ӽ���λ�ý�����һ�ֵ�ɸѡ
			raw[start]= raw[i];
			start = i;
		}

		raw[start]= temp; //ԭʼ�Ѷ��������գ����ĺ���λ��
	}
};
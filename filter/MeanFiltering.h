#pragma once 

#include "Util.h"
#include "Point.h"

class MeanFilter
{
public:
	MeanFilter() {}

	void filter(vector<Point<double>*>& src, vector<list<Point<double>*> >& mapping)
	{
		if(src.size() == 0 || src[0]->getPointType() != XYZI || src.size() != mapping.size())
		{
			return;
		}

		Util::getInstance()
				.log("start mean filter at ")
				.log(Util::getInstance().getCurrentTime())
				.endl();
		
		for(int j = 0; j < src.size(); ++j)
		{
			Point<double>* dest = src[j];

			vector<double> pData (4);
			Point<double> newPoint (pData);			

			for(list<Point<double>*>::iterator it = mapping[j].begin(); it != mapping[j].end(); ++it)
			{
				Point<double>* iPoint = *it;

				for(int dim = 0; dim < newPoint.getDimensionAmount() - 1; ++dim)
				{
					//对某维度进行卷积
					double setValue = newPoint.getValue(dim) + iPoint->getValue(dim);
					newPoint.setValue(dim, setValue);
				}
			}

			//权重和不为0，则进行单位化，获取新值
			int size = mapping[j].size();
			if(size != 0)
			{
				for(int dim = 0; dim < dest->getDimensionAmount() - 1; ++dim)
				{
					dest->setValue(dim, newPoint.getValue(dim) / size);
				}
			}
		}

		Util::getInstance()
				.log("end mean filter at ")
				.log(Util::getInstance().getCurrentTime())
				.endl();
	}
};
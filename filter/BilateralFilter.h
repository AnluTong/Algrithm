#pragma once

#include "Util.h"
#include "Point.h"

class BilateralFilter
{
public:
	BilateralFilter()
	{
		/**sigma约为窗口大小的1/2**/
		mSigmaD = 25;
		mSigmaR = 25;
	}

	/**双边滤波距离权重分布宽度**/
	BilateralFilter& setFilterSigmaD(double sigmaD)
	{
		mSigmaD = sigmaD;
		return *this;
	}

	/**双边滤波强度差权重分布宽度**/
	BilateralFilter& setFilterSigmaR(double sigmaR)
	{
		mSigmaR = sigmaR;
		return *this;
	}

	void filter(vector<Point<double>*>& src, vector<list<Point<double>*> >& mapping)
	{
		if(src.size() == 0 || src[0]->getPointType() != XYZI || src.size() != mapping.size())
		{
			return;
		}

		Util::getInstance()
				.log("start bilatera filter at ")
				.log(Util::getInstance().getCurrentTime())
				.endl();
		
		for(int j = 0; j < src.size(); ++j)
		{
			Point<double>* dest = src[j];

			vector<double> pData (4);
			Point<double> newPoint (pData);
			double totalWeight (0);
			

			for(list<Point<double>*>::iterator it = mapping[j].begin(); it != mapping[j].end(); ++it)
			{
				Point<double>* iPoint = *it;

				double intencityDiv = dest->getValue(3) - iPoint->getValue(3);			
				double weight = gauss(mSigmaD, measureDistance(*dest, *iPoint)) * gauss(mSigmaR, intencityDiv);
                totalWeight += weight;
				for(int dim = 0; dim < newPoint.getDimensionAmount() - 1; ++dim)
				{
					//对某维度进行卷积，除去强度值维度
					double setValue = newPoint.getValue(dim) + weight * iPoint->getValue(dim);
					newPoint.setValue(dim, setValue);
				}
			}

			//权重和不为0，则进行单位化，获取新值
			if(totalWeight > 0.0001 || totalWeight < -0.0001)
			{
				for(int dim = 0; dim < dest->getDimensionAmount() - 1; ++dim)
				{
					dest->setValue(dim, newPoint.getValue(dim) / totalWeight);
				}
			}
		}

		Util::getInstance()
				.log("end bilatera filter at ")
				.log(Util::getInstance().getCurrentTime())
				.endl();
	}

private:
	/**窗口高斯sigma与强度差sigma**/
	double mSigmaD;
	double mSigmaR;

	/**双边滤波两个权重因子都是高斯形式**/
	double gauss (double sigma, double distance)
	{
		return exp(-((distance * distance) / (2 * sigma * sigma)));
	}
};
#pragma once

#include "Util.h"
#include "KDTree.h"

/**
 * 各项异性扩散滤波，用于反射点强度，只能传入带强度的反射点
 * 参考实现https://github.com/stephanmg/Anisotropic-Diffusion-Filtering
 * 
 **/

class AnisotropicDiffusionFilter
{
public:
	AnisotropicDiffusionFilter() 
	{
		/**设置默认值**/
		lambda = 0.15;
		iterCount = 10;
		kappa = 5;
	}

	/**各向异性扩散和转化为强度值的系数**/
	AnisotropicDiffusionFilter& setFilterLambda(double l)
	{
		lambda = l;
		return *this;
	}

	/**扩散次数**/
	AnisotropicDiffusionFilter& setFilterIterCount(int count)
	{
		iterCount = count;
		return *this;
	}

	/**各项异性强度转换的系数**/
	AnisotropicDiffusionFilter& setFilterKappa(double k)
	{
		kappa = k;
		return *this;
	}


	void filter(vector<Point<double>*>& src, vector<list<Point<double>*> >& mapping)
	{
		if(src.size() == 0 || src[0]->getPointType() != XYZI || src.size() != mapping.size())
		{
			return;
		}

		int count (0);
		while(count < iterCount)
		{
			Util::getInstance()
				.log("It's ")
				.log(count)
				.log(" iterator")
				.endl();

			++count;
			for(int j = 0; j < src.size(); ++j)
			{
				Point<double>* dest = src[j];
				double sum (0), newValue (0);

				for(list<Point<double>*>::iterator it = mapping[j].begin(); it != mapping[j].end(); ++it)
				{
					Point<double>* iPoint = *it;

					double div = dest->getValue(3) - iPoint->getValue(3);
					double cDiv = fluxDerivative(div);
					sum += div * cDiv;
				}

				/**最终数据变化**/
				newValue = dest->getValue(3) + lambda * sum;
				dest->setValue(3, newValue);
			}
		}
	}

protected:
	virtual double fluxDerivative(double intensity) 
	{
		return exp(-pow((intensity/kappa), 2));
	}

private:
	double lambda;
	double kappa;
	int iterCount;
};
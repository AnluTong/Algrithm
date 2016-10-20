#pragma once

#include <vector>
/**
 *   点模型，各维度描述分别是 x，y，z，i或x，y，z或x，y，z，r，g，b
 **/

#define UNKNOWN -1
#define XYZ 0
#define XYZI 1
#define XYZRGB 2

template <class T>
class Point
{
public:
	Point(std::vector<T> in) {values = in;}
	T getValue(int dimnesion){ return values[dimnesion];}
	void setValue(int dimension, T value) {	values[dimension] = value;}

	int getDimensionAmount() {return values.size();}

	int getPointType()
	{
		if(values.size() == 3)
		{
			return XYZ;
		}
		else if(values.size() == 4)
		{
			return XYZI;
		}
		else if(values.size() == 6)
		{
			return XYZRGB;
		}
		else 
		{
			return UNKNOWN;
		}
	}

private:
	std::vector<T> values;
};

/**具体计算距离方法**/
double measureDistance(Point<double>& point1, Point<double>& point2)
{
	int dim = -1;
	if((dim = point1.getDimensionAmount()) != point2.getDimensionAmount())
	{
		return 0;
	}
	double res(0);
	for(int i = 0; i < dim; ++i)
	{
		res += (point1.getValue(i) - point2.getValue(i)) 
			* (point1.getValue(i) - point2.getValue(i));
	}

	return sqrt(res);
}
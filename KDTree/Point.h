#pragma once

#include <vector>

template <class T>
class Point
{
public:
	Point(std::vector<T> in) {values = in;}
	T getValue(int dimnesion){ return values[dimnesion];}

	int getDimensionAmount() {return values.size();}

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
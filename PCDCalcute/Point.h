#pragma once

class Point
{
public:
	double x, y, z;

	double getValue(int dimnesion)
	{
		switch(dimnesion)
		{
		case 0 :
			return x;
		case 1:
			return y;
		default:
			return z;
		}
	}

	static int getDimensionAmount()
	{
		return 3;
	}
};
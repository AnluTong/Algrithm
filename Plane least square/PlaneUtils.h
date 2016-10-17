#pragma once
#include "Point.h"
#include "MatrixUtils.h"
#include <vector>

using namespace std;

class PlaneUtils
{
public:
	class PlaneCoef
	{
	public:
		double A, B, C;
	};

	/**
	* 空间平面表示 A0x + A1y + A2z + 1 = 0
	* 返回值为0正常,返回值为-1异常
	**/
	static void getNearestPlane(vector<Point*>& cloud, PlaneCoef& result)
	{
		double MatrixLeft[3][3] = {0}, MatrixLeftReverse[3][3] = {0}, MatrixRight[3] = {0}, tempResult[3] = {0};

		for(vector<Point*>::iterator it = cloud.begin(); it != cloud.end(); ++it)
		{
			Point &p = **it;
			MatrixLeft[0][0] += p.x * p.x;
			MatrixLeft[0][1] += p.x * p.y;
			MatrixLeft[0][2] += p.x * p.z;

			MatrixLeft[1][0] += p.y * p.x;
			MatrixLeft[1][1] += p.y * p.y;
			MatrixLeft[1][2] += p.y * p.z;

			MatrixLeft[2][0] += p.z * p.x;
			MatrixLeft[2][1] += p.z * p.y;
			MatrixLeft[2][2] += p.z * p.z;

			MatrixRight[0] += p.x;
			MatrixRight[1] += p.y;
			MatrixRight[2] += p.z;
		}

		for(size_t j = 0; j < 3; ++j)
		{
			MatrixRight[j] = -MatrixRight[j];
		}

		MatrixUtils::GetMatrixInverse(MatrixLeft, 3, MatrixLeftReverse);

		for(int i = 0; i < 3; ++i) 
		{
			for(int j = 0; j < 3; ++j)
			{
				tempResult[i] += MatrixLeftReverse[i][j] * MatrixRight[j];
			}
		}
		result.A = tempResult[0];
		result.B = tempResult[1];
		result.C = tempResult[2];
	}
};
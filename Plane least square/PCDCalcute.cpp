// PCDCalcute.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <vector>
#include <fstream>
#include "Point.h"
#include "PlaneUtils.h"

using namespace std;

#define MAX_DISTANCE 50

vector<Point*> anlysisFile(string path, vector<Point*>& dest);

int _tmain(int argc, _TCHAR* argv[])
{
	vector<Point*> dest;
	vector<Point*> raw = anlysisFile("sick.pcd", dest);

	//最小二乘法进行平面拟合
	PlaneUtils::PlaneCoef coef;
	PlaneUtils::getNearestPlane(dest, coef);

	//计算所有点与拟合平面间距离矢量，取最大与最小之差。
	double minD(99999), maxD(0);
	for(vector<Point*>::iterator it = dest.begin(); it != dest.end(); ++it)
	{
		Point& p = **it;
		double value = 
			(coef.A * p.x + coef.B * p.y + coef.C * p.z + 1) 
			/ sqrt( coef.A * coef.A + coef.B * coef.B + coef.C * coef.C);
		if(value < minD)
		{
			minD = value;
		}

		if(value > maxD)
		{
			maxD = value;
		}
	}

	printf("max distance is %5.3f, min distance is %5.3f, statical error is %5.3f \n", maxD, minD, maxD - minD);
	getchar();

	//释放资源
	for(vector<Point*>::iterator it = raw.begin(); it != raw.end(); ++it)
	{
		delete *it;
		*it = NULL;
	}
	vector<Point*>().swap(raw);

	return 0;
}

//扫描文件粗略获取目标集合
vector<Point*> anlysisFile(string path, vector<Point*>& dest)
{
	vector<Point*> delList;
	fstream file(path);
	string buff;
	double getX(0), getY(0), getZ(0), dis(0), last(0);
	bool start = false, reach = false;
	while(!file.eof())
	{
		if(!reach)
		{
			getline(file, buff);
			reach = (string::npos != buff.find("DATA"));
			continue;
		}

		file >> getX >> getY >> getZ;
		dis = sqrt(getX * getX + getY * getY);

		if(!(last > -0.5 && last < 0.5))
		{
			if(abs(dis - last) > 2900)
			{
				start = !start;
			}
			if(start)
			{
				Point* p = new Point();
				p->x = getX;
				p->y = getY;
				p->z = getZ;
				dest.push_back(p);
				delList.push_back(p);
			}
		}
		last = dis;
	}
	file.close();

	return delList;
}


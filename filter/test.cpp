// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "KDTree.h"
#include "Point.h"
#include <fstream>
#include <iostream>
#include <string>
#include "AnisotropicDiffusionFilter.h"
#include "IsolatedPointFilter.h"
#include "BilateralFilter.h"

using namespace std;

vector<Point<double>*> anlysisSICKFile(string path, vector<Point<double>*>& dest);

int _tmain(int argc, _TCHAR* argv[])
{
	vector<Point<double>*> dest;
	vector<Point<double>*> raw = anlysisSICKFile("sick.pcd", dest);
	vector<Point<double>*> filted;
	vector<list<Point<double>*> > filtedMapping;

	/**构建kd树**/
	KDTree<double> tree;
	KDTreeUtils<double>::buildKdTree(&tree, dest, 0);

	/**孤立点过滤，并生成邻域映射**/
	IsolatedPointFilter isoFilter;
	isoFilter.setFilterDistanceThreshold(50);
	isoFilter.filter(&tree, dest, filted, filtedMapping);

	/**强度各项异性滤波**/
	AnisotropicDiffusionFilter anisoFilter;
	anisoFilter.setFilterLambda(0.15)
		.setFilterIterCount(10)
		.setFilterKappa(5);
	anisoFilter.filter(filted, filtedMapping);

	/**根据双边滤波调整点云位置**/
	BilateralFilter bilateralFilter;
	bilateralFilter.setFilterSigmaD(25)
		.setFilterSigmaR(5);
	bilateralFilter.filter(filted, filtedMapping);

	//释放资源
	for(vector<Point<double>*>::iterator it = raw.begin(); it != raw.end(); ++it)
	{
		delete *it;
		*it = NULL;
	}
	vector<Point<double>*>().swap(raw);

	getchar();
	return 0;
}

//扫描文件PCD文件，SICK测试
vector<Point<double>*> anlysisSICKFile(string path, vector<Point<double>*>& dest)
{
	vector<Point<double>*> delList;
	fstream file(path);
	string buff;
	double getX(0), getY(0), getZ(0);
	bool reach = false;
	int intence (100);
	while(!file.eof())
	{
		if(!reach)
		{
			getline(file, buff);
			reach = (string::npos != buff.find("DATA"));
			continue;
		}

		file >> getX >> getY >> getZ;
		vector<double> v;
		v.push_back(getX);
		v.push_back(getY);
		v.push_back(getZ);
		v.push_back(intence);

		/**随便生成一个强度以满足要求**/
		++intence;
		if(intence == 110)
		{
			intence = 100;
		}

		Point<double>* p = new Point<double>(v);
		dest.push_back(p);
		delList.push_back(p);
	}
	file.close();

	return delList;
}
#include "stdafx.h"
#include "KDTree.h"
#include <vector>
#include <iostream>

using namespace std;

int data[][3] = {{2,3,0},{5,4,0},{9,6,0},{4,7,0},{8,1,0},{7,2,0},{8,8,1},{20,7,1}};

int main()
{
	KDTree<double>* tree = new KDTree<double>();

	vector<Point<double>* > cloud;
	for (size_t i = 0; i < 8; ++i)
	{
		vector<double> value;
		value.push_back(data[i][0]);
		value.push_back(data[i][1]);
		value.push_back(data[i][2]);

		Point<double>* p = new Point<double>(value);
		cloud.push_back(p);
	}

	KDTreeUtils<double>::buildKdTree(tree, cloud, 0);
	vector<double> tstPoint;
	tstPoint.push_back(10);
	tstPoint.push_back(4.5);
	tstPoint.push_back(0);
	Point<double> dest (tstPoint);
	Point<double>* result = KDTreeUtils<double>::searchNearestNeighbor(&dest, tree);

	cout << "The nearest neighbor is: " << result->getValue(0) << ", "<<result->getValue(1)<<", "<<result->getValue(2)<<endl;
	cout << "The k nearest neighbor is: "<< endl;

	vector<Point<double>*> resultSet = KDTreeUtils<double>::knn(&dest, tree, 5, 200);
	for(size_t i = 0; i < resultSet.size(); ++i)
	{
		cout << resultSet[i]->getValue(0) << ", "<<resultSet[i]->getValue(1)<<", "<<resultSet[i]->getValue(2)<<endl;
	}

	getchar();
	return 0;
}
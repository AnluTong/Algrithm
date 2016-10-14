#include "stdafx.h"
#include "KMean.h"

#define MAX_CIRCLE_COUNT 100

void KMean::initCentroids()
{
	//初始化centroids
	vector<vector<double> >().swap(centroids);
	vector<double> vec(DIM_SIZE, 0);
	for (int i = 0; i < K_SIZE; ++i)
	{
		centroids.push_back(vec);
	}

	// set values by column
	srand(time(NULL));
	for (int j = 0; j < DIM_SIZE; ++j)
	{
		//获取第j维最大最小值，并设置k个聚类为该范围内的随机值
		//获取idx维度的最大和最小值
		int idx = j;
		double min, max;
		dataSet[0].at(idx) > dataSet[1].at(idx) 
			? ( max = dataSet[0].at(idx), min = dataSet[1].at(idx) ) 
			: ( max = dataSet[1].at(idx), min = dataSet[0].at(idx) );

		for (int k = 2; k < DATE_SIZE; ++k)
		{
			if (dataSet[k].at(idx) < min)
				min = dataSet[k].at(idx);
			else if (dataSet[k].at(idx) > max)
				max = dataSet[k].at(idx);
			else
				continue;
		}

		//初始化聚类中心
		double rangeIdx = max - min;
		for (int k = 0; k < K_SIZE; ++k)
		{
			// generate float data between 0 and 1
			centroids[k].at(idx) = min + rangeIdx * ( rand() / (double)RAND_MAX);
		}
	}
}
void KMean::initClusterAssignment()
{
	vector<Node>().swap(clusterAssignment);
	Node node(-1, -1);
	for (int i = 0; i < DATE_SIZE; ++i)
	{
		clusterAssignment.push_back(node);
	}
}

double KMean::distEclud(vector<double> &v1, vector<double> &v2)
{
	double sum = 0;
	int size = v1.size();
	for (int i = 0; i < size; ++i)
	{
		sum += (v1[i] - v2[i]) * (v1[i] - v2[i]);
	}
	return sum;
}

void KMean::init(const vector<vector<double> >& data, int k)
{
	vector<vector<double> >().swap(dataSet);
	dataSet = data;
	K_SIZE = k;
	DIM_SIZE = dataSet[0].size();
	DATE_SIZE = dataSet.size();

	initClusterAssignment();
	initCentroids();
}

void KMean::clusterImpl(const vector<vector<double> >& data, int k)
{
	init(data, k);

	bool clusterChanged = true;
	int count (0);
	while (clusterChanged && count < MAX_CIRCLE_COUNT)
	{
		++count;
		clusterChanged = false;
		//比较距离，将所有点进行分类，并更新辅助节点数组
		cout << "find the nearest centroid of each point : " << endl;
		for (int i = 0; i < DATE_SIZE; ++i)
		{
			int minIndex = -1;
			double minDist = INT_MAX;
			for (int j = 0; j < K_SIZE; ++j)
			{
				// 与最近的质心聚为一类
				double distJI = distEclud(centroids[j], dataSet[i]);
				if ( distJI < minDist ) // 只取最短距离
				{
					minDist = distJI;
					minIndex = j;
				}
			}
			//索引与距离任意一个都不相同都要更新距离值
			if ( clusterAssignment[i].minIndex != minIndex || clusterAssignment[i].minDist != minDist)
			{
				clusterChanged = true;
				clusterAssignment[i].minIndex = minIndex;
				clusterAssignment[i].minDist = minDist;
			}
		}

		//更新聚类中心
		cout << "update the centroids : " << endl;
		vector<double> vec(DIM_SIZE, 0);
		vector<vector<double> > vecList(K_SIZE, vec);
		vector<int> countList(K_SIZE, 0);
		for(int i = 0; i < DATE_SIZE; ++i)
		{
			//获取该点对应的聚类索引
			int clusterIdx =  clusterAssignment[i].minIndex;
			//统计该聚类点的个数
			++countList[clusterIdx];
			//对该聚类各维度进行求和
			for(int dim = 0; dim < DIM_SIZE; ++dim)
			{
				vecList[clusterIdx][dim] += dataSet[i][dim];
			}
		}

		for(int i = 0; i < K_SIZE; ++i)
		{
			for( int dim = 0; dim < DIM_SIZE; ++dim)
			{
				//对聚类某一维度求均值，并以此作为新的聚类中心
				if (countList[i] != 0)
				{
					vecList[i][dim] /= countList[i];
				}
				centroids[i][dim] = vecList[i][dim]; // 质心是虚拟的
			}
		}
	}
}

vector<Cluster> KMean::devideCluster()
{
	Cluster cluster;
	vector<Cluster> result (K_SIZE, cluster);
	for(int i = 0; i < DATE_SIZE; ++i)
	{
		//获取该点对应的聚类索引
		int clusterIdx =  clusterAssignment[i].minIndex;
		vector<double>& point = dataSet[i];
		//对点进行分类
		result[clusterIdx].support.push_back(point);
		//设置聚类中心
		result[clusterIdx].mode = centroids[clusterIdx];
	}

	//删除空聚类
	for(vector<Cluster>::iterator it = result.begin(); it != result.end();)
	{
		Cluster& clu = *it;
		if(clu.support.size() == 0)
		{
			it = result.erase(it);
		}
		else
		{
			++it;
		}
	}

	return result;
}

vector<Cluster> KMean::cluster(const vector<vector<double> >& data, int k)
{
	clusterImpl(data, k);
	return devideCluster();
}

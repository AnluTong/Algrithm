#pragma once
#include <vector>
#include <iostream>
#include <set>
#include "Cluster.h"
#include <ctime> // for srand

using namespace std;

class KMean
{
private:
		vector<vector<double> > dataSet; // 数据集合
		int DIM_SIZE, DATE_SIZE; // 数据维度及数据数量
		int K_SIZE; //划分为K个聚类
		vector<vector<double> > centroids; //K个聚类中心
		class Node //辅助节点
		{
		public:
			int minIndex; // 属于哪个质心的类别
			double minDist; // 与最近质心的距离
			Node(int idx, double dist):minIndex(idx),minDist(dist) {}
		};
		vector<Node> clusterAssignment; //辅助节点数组，用于存储每个点属于的聚类中心及两者间距离，一一对应于dataSet
	
		void initCentroids(); //初始化聚类中心的某一维度值
		void initClusterAssignment(); //初始化辅助节点数组
		double distEclud(vector<double> &v1, vector<double> &v2); //测量两点的欧拉距离

		void init(const vector<vector<double> >& data, int k); //初始化接口
		void clusterImpl(const vector<vector<double> >& data, int k);
		vector<Cluster> devideCluster();
public:
		KMean() {}
		vector<Cluster> cluster(const vector<vector<double> >& data, int k);
};


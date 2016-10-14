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
		vector<vector<double> > dataSet; // ���ݼ���
		int DIM_SIZE, DATE_SIZE; // ����ά�ȼ���������
		int K_SIZE; //����ΪK������
		vector<vector<double> > centroids; //K����������
		class Node //�����ڵ�
		{
		public:
			int minIndex; // �����ĸ����ĵ����
			double minDist; // ��������ĵľ���
			Node(int idx, double dist):minIndex(idx),minDist(dist) {}
		};
		vector<Node> clusterAssignment; //�����ڵ����飬���ڴ洢ÿ�������ڵľ������ļ����߼���룬һһ��Ӧ��dataSet
	
		void initCentroids(); //��ʼ���������ĵ�ĳһά��ֵ
		void initClusterAssignment(); //��ʼ�������ڵ�����
		double distEclud(vector<double> &v1, vector<double> &v2); //���������ŷ������

		void init(const vector<vector<double> >& data, int k); //��ʼ���ӿ�
		void clusterImpl(const vector<vector<double> >& data, int k);
		vector<Cluster> devideCluster();
public:
		KMean() {}
		vector<Cluster> cluster(const vector<vector<double> >& data, int k);
};


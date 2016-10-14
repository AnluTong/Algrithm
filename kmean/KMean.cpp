#include "stdafx.h"
#include "KMean.h"

#define MAX_CIRCLE_COUNT 100

void KMean::initCentroids()
{
	//��ʼ��centroids
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
		//��ȡ��jά�����Сֵ��������k������Ϊ�÷�Χ�ڵ����ֵ
		//��ȡidxά�ȵ�������Сֵ
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

		//��ʼ����������
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
		//�ȽϾ��룬�����е���з��࣬�����¸����ڵ�����
		cout << "find the nearest centroid of each point : " << endl;
		for (int i = 0; i < DATE_SIZE; ++i)
		{
			int minIndex = -1;
			double minDist = INT_MAX;
			for (int j = 0; j < K_SIZE; ++j)
			{
				// ����������ľ�Ϊһ��
				double distJI = distEclud(centroids[j], dataSet[i]);
				if ( distJI < minDist ) // ֻȡ��̾���
				{
					minDist = distJI;
					minIndex = j;
				}
			}
			//�������������һ��������ͬ��Ҫ���¾���ֵ
			if ( clusterAssignment[i].minIndex != minIndex || clusterAssignment[i].minDist != minDist)
			{
				clusterChanged = true;
				clusterAssignment[i].minIndex = minIndex;
				clusterAssignment[i].minDist = minDist;
			}
		}

		//���¾�������
		cout << "update the centroids : " << endl;
		vector<double> vec(DIM_SIZE, 0);
		vector<vector<double> > vecList(K_SIZE, vec);
		vector<int> countList(K_SIZE, 0);
		for(int i = 0; i < DATE_SIZE; ++i)
		{
			//��ȡ�õ��Ӧ�ľ�������
			int clusterIdx =  clusterAssignment[i].minIndex;
			//ͳ�Ƹþ����ĸ���
			++countList[clusterIdx];
			//�Ըþ����ά�Ƚ������
			for(int dim = 0; dim < DIM_SIZE; ++dim)
			{
				vecList[clusterIdx][dim] += dataSet[i][dim];
			}
		}

		for(int i = 0; i < K_SIZE; ++i)
		{
			for( int dim = 0; dim < DIM_SIZE; ++dim)
			{
				//�Ծ���ĳһά�����ֵ�����Դ���Ϊ�µľ�������
				if (countList[i] != 0)
				{
					vecList[i][dim] /= countList[i];
				}
				centroids[i][dim] = vecList[i][dim]; // �����������
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
		//��ȡ�õ��Ӧ�ľ�������
		int clusterIdx =  clusterAssignment[i].minIndex;
		vector<double>& point = dataSet[i];
		//�Ե���з���
		result[clusterIdx].support.push_back(point);
		//���þ�������
		result[clusterIdx].mode = centroids[clusterIdx];
	}

	//ɾ���վ���
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

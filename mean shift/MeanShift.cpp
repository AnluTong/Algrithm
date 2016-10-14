#include "stdafx.h"
#include "MeanShift.h"

#define SHIFT_COEF 0.01
#define MAX_CIRCLE_COUNT 100
#define CLUSTER_COEF 0.5

void MeanShift::setKernel( double (*fun)(double,double) ) 
{
    if(!fun)
	{
        kernelFunc = gaussianKernel;
    } else {
        kernelFunc = fun;    
    }
}

vector<double> MeanShift::shiftPoint(vector<double>& point, vector<vector<double> >& clouds, double bandwidth)
{
	vector<double> newPoint(point.size(), 0);
    double total_weight = 0;
    
	for(int i = 0; i < clouds.size(); ++i)
	{
        double weight = kernelFunc(eulaDistance(point, clouds[i]), bandwidth);
        for(int j=0; j < newPoint.size(); ++j)
		{
            newPoint[j] += clouds[i][j] * weight;
        }
        total_weight += weight;
    }

    for(int i = 0; i < newPoint.size(); ++i)
	{
		(total_weight != 0) ? newPoint[i] /= total_weight : newPoint[i] = point[i];
    }
    return newPoint;
}

vector<Cluster> MeanShift::cluster(vector<vector<double> >& points, double bandwidth)
{
	vector<Cluster> clusters;

	for(int i = 0; i < points.size(); ++i) 
	{
		vector<double> oldMean = points[i];

		int count = 0;
		while(count < MAX_CIRCLE_COUNT) //设置最大循环次数
		{
			//对该点进行偏移
			vector<double> newMean = shiftPoint(oldMean, points, bandwidth);
			
			bool isStop = eulaDistance(oldMean, newMean) < SHIFT_COEF * bandwidth;
			oldMean = newMean;
			++count;

			if(isStop) //如果偏移后的点与原先点的距离小于阈值，则终止
			{
				break;
			}
		}

		int j (0);
		for(j = 0; j < clusters.size(); ++j) //遍历已有的cluster，看属于哪个聚类
		{
			if(eulaDistance(oldMean, clusters[j].mode) < CLUSTER_COEF * bandwidth) //找到
				break;
		}

		if(j == clusters.size()) //没找到
		{
			Cluster clus;
            clus.mode = oldMean;
            clusters.push_back(clus); //新增聚类
		}

		clusters[j].support.push_back(points[i]); //将已有点加入聚类
	}

	return clusters;
}
#pragma once 

#include <vector>
#include "Cluster.h"
#include <stdio.h>
#include <math.h>

using namespace std;

class MeanShift 
{
public:
	MeanShift() { setKernel(NULL); }
	MeanShift(double (*_kernel_func)(double,double)) { setKernel(kernelFunc); }

	vector<Cluster> cluster(vector<vector<double> >& points, double bandwidth);

private:
	/**设置核函数**/
	double (*kernelFunc)(double,double);
	void setKernel(double (*fun)(double,double));

	vector<double> shiftPoint(vector<double>& oldPoint, vector<vector<double> >& clouds, double bandwidth);

	static double eulaDistance(vector<double>& point_a, vector<double>& point_b)
	{
		double total = 0;
		for(int i = 0; i < point_a.size(); ++i)
		{
			total += (point_a[i] - point_b[i]) * (point_a[i] - point_b[i]);
		}
		return sqrt(total);
	}

	static double gaussianKernel(double distance, double kernel_bandwidth)
	{
		return exp(-0.5 * (distance*distance) / (kernel_bandwidth*kernel_bandwidth));
	}
};

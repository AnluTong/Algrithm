#pragma once
#include "KDTree.h"
#include "Util.h"

class IsolatedPointFilter
{
public:
	IsolatedPointFilter()
	{
		distanceThreshold = 50;
	}

	/**点云误差阈值，大于则排除**/
	IsolatedPointFilter& setFilterDistanceThreshold(double d)
	{
		distanceThreshold = d;
		return *this;
	}

	void filter(KDTree<double>* tree, vector<Point<double>*>& src, vector<Point<double>*>& result, vector<list<Point<double>*> >& mapping)
	{
		if(src.size() == 0 || src[0]->getPointType() != XYZI )
		{
			return;
		}

		/**mapping为k邻域映射，mapping大小与result相同**/
		Util::getInstance()
			.log("create mapping, time is ")
			.log(Util::getInstance().getCurrentTime())
			.endl();

		/**需要被滤除**/
		hash_set<Point<double>*> delSet;
		for(int i = 0; i < src.size(); ++i)
		{
			Point<double>* p = src[i];
			int maxSearch = 200;
			vector<Point<double>*> knnResult = KDTreeUtils<double>::knn(p, tree, 5, maxSearch);
			list<Point<double>*> knnList (knnResult.begin(), knnResult.end());
			if(knnList.size() < 2)
			{
				/**若邻域点个数小于2（包括自己），该点加入被删除列表**/
				delSet.insert(p);
				continue;
			}

			//搜索到knn点，有一个是自己本身，所以最近的应该从第1点算起
			double dis = measureDistance(*p, *knnResult[1]);
			if(dis < distanceThreshold)
			{
				/**正常情况**/
				mapping.push_back(knnList);
				/**加入结果数组**/
				result.push_back(p);
			}
			else
			{
				/**若最近的点距离大于指定值，则该点加入被删除列表**/
				delSet.insert(p);
			}
		}

		Util::getInstance()
			.log("end mapping, time is ")
			.log(Util::getInstance().getCurrentTime())
			.endl();

		Util::getInstance()
			.log("mapping adjust start time is ")
			.log(Util::getInstance().getCurrentTime())
			.endl();
		/**按照delist，对mapping进行清理**/
		for(int i = 0; i < mapping.size(); ++i)
		{
			/**存储邻域用list链表，便于删除**/
			for(list<Point<double>*>::iterator it = mapping[i].begin(); it != mapping[i].end();)
			{
				Point<double>* p = *it;
				if(delSet.end() != delSet.find(p))
				{
					it = mapping[i].erase(it);
				}
				else
				{
					++it;
				}
			}
		}

		Util::getInstance()
			.log("mapping adjust end time is ")
			.log(Util::getInstance().getCurrentTime())
			.endl();
	}

private:
	double distanceThreshold;
};
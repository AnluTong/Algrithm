#pragma once

#include <vector>
#include <algorithm>
#include <set>
#include "Point.h"
#include "ATSort.h"
#include "ATHeap.h"
#include "ATICompare.h"
#include "ATPriorityQueue.h"

using namespace std;

extern double measureDistance(Point<double>& point1, Point<double>& point2);

template<class T>
class KDTree
{
public: 
	//当前值
	Point<T>* root;
	//父节点
	KDTree<T>* parent;
	//左子树
	KDTree<T>* leftChild;
	//右子树
	KDTree<T>* rightChild;
	//当前划分维度
	int split;

	//默认构造函数
	KDTree(){parent = leftChild = rightChild = NULL; split = -1;}
	~KDTree(){freeMemory();}

	//判断kd树是否为空
	bool isEmpty(){ return root == NULL;}
	//判断kd树是否只是一个叶子结点
	bool isLeaf(){ return root != NULL && rightChild == NULL && leftChild == NULL;}
	//判断是否是树的根结点
	bool isRoot(){ return (!isEmpty()) && parent == NULL;}
	//判断该子kd树的根结点是否是其父kd树的左结点
	bool isLeft(){ if (parent->leftChild != NULL) return parent->leftChild->root == root; return false;}
	//判断该子kd树的根结点是否是其父kd树的右结点
	bool isRight() { if(parent-rightChild != NULL) return parent->rightChild->root == root; return false;}

	//从此处创建子树，可以释放内存
	KDTree<T>* generateNewTree(){ KDTree<T>* tree = new KDTree<T>(); delList.push_back(tree); return tree;}

private:
	//删除列表
	vector<KDTree<T>*> delList;

	//释放子树的内存
	void freeMemory()
	{
		for(vector<KDTree<T>*>::iterator it = delList.begin(); it != delList.end(); ++it)
		{
			if( *it != NULL)
			{
				delete *it;
				*it = NULL;
			}
		}
		vector<KDTree<T>*>().swap(delList);
	}
};

template <class T>
class KDTreeUtils
{
public:
	//构建kd树
	static void buildKdTree(KDTree<T>* tree, vector<Point<T>*> data, unsigned depth)
	{
		if(tree == NULL)
		{
			return;
		}
		//样本的数量
		unsigned samplesNum = data.size();
		//终止条件
		if (samplesNum == 0)
		{
			return;
		}
		//样本的总维度
		int k = data[0]->getDimensionAmount();

		if (samplesNum == 1)
		{
			tree->root = data[0];
			tree->split = depth % k;
			return;
		}

		//选择切分维度
		int splitAttribute = depth % k;
		//排序求中位数
		double splitValue = getMidNumOfDim(data, splitAttribute);

		// 根据选定的切分属性和切分值，将数据集分为两个子集
		vector<Point<T>*> subsetLeft;
		vector<Point<T>*> subsetRight;

		//比中位数小的放在左集合，大的放在右集合
		for(vector<Point<T>*>::iterator it = data.begin(); it != data.end(); ++it)
		{
			Point<T>* p = *it;
			if(p->getValue(splitAttribute) == splitValue 
				&& tree->isEmpty())
			{
				tree->root = p;
				tree->split = splitAttribute;
			}
			else
			{
				(p->getValue(splitAttribute) < splitValue) ? subsetLeft.push_back(p) : subsetRight.push_back(p);
			}
		}

		//子集递归调用buildKdTree函数
		if(subsetLeft.size() > 0)
		{
			tree->leftChild = tree->generateNewTree();
			tree->leftChild->parent = tree;
			buildKdTree(tree->leftChild, subsetLeft, depth + 1);
		}

		if(subsetRight.size() > 0)
		{
			tree->rightChild = tree->generateNewTree();
			tree->rightChild->parent = tree;
			buildKdTree(tree->rightChild, subsetRight, depth + 1);
		}
	}

	//在kd树tree中搜索目标点goal的最近邻
	//输入：目标点；已构造的kd树
	//输出：目标点的最近邻
	static Point<T>* searchNearestNeighbor(Point<T>* goal, KDTree<T>* tree)
	{
		vector<Point<T>*> out = knn(goal, tree, 1, 200);
		if(out.size() > 0)
		{
			return out[0];
		}
		else
		{
			return NULL;
		}
	}

	//k近邻搜索（近似），利用两个堆，一个访问结点堆，一个候选路径堆
	static vector<Point<T>*> knn(Point<T>* goal, KDTree<T>* tree, unsigned int k, int maxCount)
	{
		CompareRoutePoint routerComp (goal);
		CompareCandicatePoint candiComp (goal);

		ATPriorityQueue<Point<T>*> routerQueue (routerComp, k);
		ATPriorityQueue<KDTree<T>*> candicateQueue (candiComp, k);

		if(goal == NULL || tree == NULL || k < 0)
		{
			return routerQueue.allData();
		}

		candicateQueue.add(tree);
		int cycleCount = 0;

		while(candicateQueue.size() > 0 && cycleCount < maxCount)
		{
			KDTree<T>* out = candicateQueue.poll();

			searchLeafWithHeap(goal, out, k, routerQueue, candicateQueue);

			++cycleCount;
		}

		vector<Point<T>*> result = routerQueue.allData();

		ATSort<Point<T>*>::sort(result, routerComp);
		return result;
	}

	//r半径搜索（近似），同样利用两个堆，一个访问结点堆，一个候选路径堆
	static vector<Point<T>*> radiusSearch(Point<T>* goal, KDTree<T>* tree, T radius, int maxCount)
	{
		vector<Point<T>*> routerList; // 路径列表，存储小于半径的点
		vector<KDTree<T>*> candiList; // 候选路径列表

		if(goal == NULL || tree == NULL || radius < 0)
		{
			return routerList;
		}

		enterRadiusSearchCandicate(goal, tree, radius, candiList);
		int cycleCount = 0;

		while(candiList.size() > 0 && cycleCount < maxCount)
		{
			if(candiList.size() == 0)
			{
				break;
			}

			KDTree<T>* out = candiList[candiList.size() - 1];
			candiList.pop_back();
			radiusSearchLeafWithVector(goal, out, radius, routerList, candiList);

			++cycleCount;
		}

		//输出无序的列表
		return routerList;
	}

private:
	//建树，对某维度的所有值进行排序
	class CompareDimValue : public ATICompare<Point<T>*>
	{
	public:
		CompareDimValue(int dim) {sortDimension = dim;}

		virtual bool compare(Point<T>*& left, Point<T>*& right)
		{
			return left->getValue(sortDimension) < right->getValue(sortDimension);
		}
	private:
		int sortDimension;
	};

	//对访问结点的堆进行排序
	class CompareRoutePoint : public ATICompare<Point<T>*>
	{
	public:
		CompareRoutePoint(Point<T>* dest) {sortGoal = dest;}

		virtual bool compare(Point<T>*& left, Point<T>*& right)
		{
			return measureDistance(*sortGoal, *left) <measureDistance(*sortGoal, *right);
		}
	private:
		Point<T>* sortGoal;
	};

	//对候选路径堆进行排序
	class CompareCandicatePoint : public ATICompare<KDTree<T>*>
	{
	public:
		CompareCandicatePoint(Point<T>* dest) {sortGoal = dest;}

		virtual bool compare(KDTree<T>*& left, KDTree<T>*& right)
		{
			/**目标点与超平面距离作为候选点优先级排序标准**/
			if(left->root != NULL && right->root != NULL)
			{
				return abs(left->root->getValue(left->split) - sortGoal->getValue(left->split))
					< abs(right->root->getValue(right->split) - sortGoal->getValue(right->split));
			}
			return false;
		}
	private:
		Point<T>* sortGoal;
	};

	//建树中，返回某维度的中位数
	static double getMidNumOfDim(vector<Point<T>*>& src, int dimension)
	{
		CompareDimValue compValue (dimension);

		if(src.size() == 0)
		{
			return 0;
		}

		int k = (src.size() - 1) / 2 + 1;
		ATSort<Point<T>*>::getMaxK(src, k, compValue);
		return src[src.size() - k]->getValue(dimension);
	}

	//knn从已知位置搜索到叶子结点
	static KDTree<T>* searchLeafWithHeap(Point<T>* goal, KDTree<T> *tree, int k, ATPriorityQueue<Point<T>*>& router, ATPriorityQueue<KDTree<T>*>& candicater)
	{
		KDTree<T>* currentTree = tree;
		router.add(currentTree->root); //路径入堆

		int tempDim (-1);
		while(currentTree != NULL && !currentTree->isLeaf())
		{
			tempDim = currentTree->split;
			if (goal->getValue(tempDim) < currentTree->root->getValue(tempDim))
			{
				if(currentTree->leftChild != NULL)
				{
					if(currentTree->rightChild != NULL)
					{
						candicater.add(currentTree->rightChild);//另一分支作为候选点入堆
					}

					currentTree = currentTree->leftChild;
					router.add(currentTree->root); //路径入堆
				}
				else
				{
					if(currentTree->isLeaf())
					{
						break;
					}
					else
					{
						currentTree = currentTree->rightChild;
						router.add(currentTree->root); //路径入堆
					}
				}
			}
			else
			{
				if(currentTree->rightChild != NULL)
				{
					if(currentTree->leftChild != NULL)
					{
						candicater.add(currentTree->leftChild); //另一分支作为候选点入堆
					}

					currentTree = currentTree->rightChild;
					router.add(currentTree->root);//路径入堆
				}
				else
				{
					if(currentTree->isLeaf())
					{
						break;
					}
					else
					{
						currentTree = currentTree->leftChild;
						router.add(currentTree->root);//路径入堆
					}
				}
				
			}
		}

		return currentTree;
	}

	//radius search从已知位置搜索到叶子结点
	static KDTree<T>* radiusSearchLeafWithVector(Point<T>* goal, KDTree<T> *tree, T radius, vector<Point<T>*>& router, vector<KDTree<T>*>& candicater)
	{
		KDTree<T>* currentTree = tree;
		enterRadiusSearchRouter(goal, currentTree->root, radius, router); //路径入数组

		int tempDim (-1);
		while(currentTree != NULL && !currentTree->isLeaf())
		{
			tempDim = currentTree->split;
			if (goal->getValue(tempDim) < currentTree->root->getValue(tempDim))
			{
				if(currentTree->leftChild != NULL)
				{
					if(currentTree->rightChild != NULL)
					{
						enterRadiusSearchCandicate(goal, currentTree->rightChild, radius, candicater); //另一分支作为候选点进入数组
					}

					currentTree = currentTree->leftChild;
					enterRadiusSearchRouter(goal, currentTree->root, radius, router); //路径入数组
				}
				else
				{
					if(currentTree->isLeaf())
					{
						break;
					}
					else
					{
						currentTree = currentTree->rightChild;
						enterRadiusSearchRouter(goal, currentTree->root, radius, router); //路径入数组
					}
				}
			}
			else
			{
				if(currentTree->rightChild != NULL)
				{
					if(currentTree->leftChild != NULL)
					{
						enterRadiusSearchCandicate(goal, currentTree->leftChild, radius, candicater); //另一分支作为候选点进入数组
					}

					currentTree = currentTree->rightChild;
					enterRadiusSearchRouter(goal, currentTree->root, radius, router); //路径入数组
				}
				else
				{
					if(currentTree->isLeaf())
					{
						break;
					}
					else
					{
						currentTree = currentTree->leftChild;
						enterRadiusSearchRouter(goal, currentTree->root, radius, router); //路径入数组
					}
				}
				
			}
		}

		return currentTree;
	}

	//radius search 进入路径数组
	static void enterRadiusSearchRouter(Point<T>* goal, Point<T> *point, T radius, vector<Point<T>*>& routeList)
	{
		if(goal == NULL || point == NULL)
		{
			return;
		}

		//目标点到点的距离小于R，进入路径数组
		if(measureDistance(*goal, *point) < radius)
		{
			routeList.push_back(point);
		}
	}

	//radius search 进入候选数组
	static void enterRadiusSearchCandicate(Point<T>* goal, KDTree<T>* tree, T radius, vector<KDTree<T>*>& candiList)
	{
		if(goal == NULL || tree == NULL || tree->root == NULL || tree->split == -1)
		{
			return;
		}

		//目标点到超平面距离小于R，可以进入候选数组
		int dim = tree->split;
		if(abs(goal->getValue(dim) - tree->root->getValue(dim)) < radius)
		{
			candiList.push_back(tree);
		}
	}

	//原始的最近邻搜索算法，非BBF
	//static Point<T>* nearestNeighborImpl(Point<T>* goal, KDTree<T> *tree, set<Point<T>*>& visitedSets)
	//{
	//	Point<T>* currentNearest = tree->root;
	//	KDTree<T>* currentTree = searchLeaf(goal, tree);
	//	currentNearest = currentTree->root;


	//	//如果搜索区域对应的子kd树的根结点不是整个kd树的根结点，继续回退搜索
	//	while (currentTree->parent != NULL)
	//	{
	//		//左子树存在，且未访问过则一直向左移动
	//		if(currentTree->leftChild != NULL 
	//			&& !isVisited(visitedSets, currentTree->leftChild->root))
	//		{
	//			currentTree = currentTree->leftChild;
	//			continue;
	//		}
	//		else
	//		{

	//			//若更小，则更新最近点
	//			if(measureDistance(*goal, *currentTree->root) < measureDistance(*goal, *currentNearest))
	//			{
	//				currentNearest = currentTree->root;
	//			}
	//			setVisited(visitedSets, currentTree->root); //已经访问

	//			//搜索区域的维度及该维度下超平面的值
	//			int searchDim = currentTree->parent->split;
	//			double superPlaneValue = currentTree->parent->root->getValue(searchDim);
	//			//目标点到超平面的距离
	//			double districtDistance = abs(goal->getValue(searchDim) - superPlaneValue);


	//			KDTree<T>* anotherTree;
	//			//左右子树切换
	//			currentTree->isLeft() ? anotherTree = currentTree->parent->rightChild : anotherTree = currentTree->parent->leftChild;
	//			//如果“目标点到超平面的距离”比“目标点到当前最近点距离”短，表明父节点或
	//			//另一子树可能存在距离目标点更近的点
	//			if(districtDistance < measureDistance(*goal, *currentNearest))
	//			{
	//				if(anotherTree != NULL && !isVisited(visitedSets, anotherTree->root))
	//				{
	//					//若存在或未访问则将当前树切换过去
	//					currentTree = anotherTree;
	//					continue;
	//				}
	//				else
	//				{
	//					currentTree = currentTree->parent;
	//				}
	//			}
	//			else
	//			{
	//				if(anotherTree != NULL)
	//				{
	//					//另一边子树明显更大，设置为已访问（防止再次迭代）
	//					setVisited(visitedSets, anotherTree->root); //已经访问
	//				}
	//				currentTree = currentTree->parent;
	//			}
	//		}
	//	}
	//	return currentNearest;
	//}
};
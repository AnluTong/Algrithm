#pragma once

#include <vector>
#include <algorithm>
#include <set>
#include "Point.h"

using namespace std;

class KDTree
{
public: 
	//当前值
	Point* root;
	//父节点
	KDTree* parent;
	//左子树
	KDTree* leftChild;
	//右子树
	KDTree* rightChild;
	//当前划分维度
	int split;

	//默认构造函数
	KDTree(){parent = leftChild = rightChild = NULL;}
	~KDTree(){freeMemory();}

	//判断kd树是否为空
	bool isEmpty()
	{
		return root == NULL;
	}

	//判断kd树是否只是一个叶子结点
	bool isLeaf()
	{
		return root != NULL 
			&& rightChild == NULL 
			&& leftChild == NULL;
	}

	//判断是否是树的根结点
	bool isRoot()
	{
		return (!isEmpty()) 
			&& parent == NULL;
	}

	//判断该子kd树的根结点是否是其父kd树的左结点
	bool isLeft()
	{
		if(parent->leftChild != NULL)
		{
			return parent->leftChild->root == root;
		}
		return false;
	}

	//判断该子kd树的根结点是否是其父kd树的右结点
	bool isRight()
	{
		if(parent-rightChild != NULL)
		{
			return parent->rightChild->root == root;
		}
		return false;
	}

public:
	//构建kd树
	static void buildKdTree(KDTree* tree, vector<Point*> data, unsigned depth)
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
		if (samplesNum == 1)
		{
			tree->root = data[0];
			tree->split = depth % Point::getDimensionAmount();
			return;
		}

		//样本的总维度
		int k = Point::getDimensionAmount();
		//选择切分维度
		int splitAttribute = depth % k;
		//排序求中位数
		double splitValue = getMidNumOfDim(data, splitAttribute);

		// 根据选定的切分属性和切分值，将数据集分为两个子集
		vector<Point*> subsetLeft;
		vector<Point*> subsetRight;

		//比中位数小的放在左集合，大的放在右集合
		for(vector<Point*>::iterator it = data.begin(); it != data.end(); ++it)
		{
			Point* p = *it;
			if(p->getValue(splitAttribute) == splitValue 
				&& tree->isEmpty())
			{
				tree->root = p;
				tree->split = depth % Point::getDimensionAmount();
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
	static Point* searchNearestNeighbor(Point* goal, KDTree *tree)
	{
		set<Point*> visitedSets;//元素是否访问过

		return nearestNeighborImpl(goal, tree, visitedSets);
	}

	//k近邻搜索，直接从set中获取，若个数小于k则直接返回set中已有的
	//可能不太准确，最准确的方式是进行k次最近邻搜索
	static vector<Point*> knn(Point* goal, KDTree* tree, unsigned int k)
	{
		set<Point*> visitedSets;//元素是否访问过
		nearestNeighborImpl(goal, tree, visitedSets);

		vector<Point*> points (visitedSets.begin(), visitedSets.end());
		CompareSetPoint sortSets (goal);
		quicksort(points, 0, points.size() - 1, sortSets);

		if(k < points.size())
		{
			vector<Point*> result;
			for(size_t i = 0; i < k; ++i)
			{
				result.push_back(points[i]);
			}
			return result;
		}
		else
		{
			return points;
		}
	}

	static double measureDistance(Point& point1, Point& point2)
	{
		double res(0);
		for(int i = 0; i < Point::getDimensionAmount(); ++i)
		{
			res += pow(point1.getValue(i) - point2.getValue(i), 2);
		}

		return sqrt(res);
	}

private:
	class ICompare
	{
	public:
		virtual bool comp(Point*& left, Point*& right) = 0;
	};

	class CompareValue : public ICompare
	{
	public:
		CompareValue(int dim) {sortDimension = dim;}

		virtual bool comp(Point*& left, Point*& right)
		{
			return left->getValue(sortDimension) < right->getValue(sortDimension);
		}
	private:
		int sortDimension;
	};

	class CompareSetPoint : public ICompare
	{
	public:
		CompareSetPoint(Point* dest) {sortGoal = dest;}

		virtual bool comp(Point*& left, Point*& right)
		{
			return measureDistance(*sortGoal, *left) <measureDistance(*sortGoal, *right);
		}
	private:
		Point* sortGoal;
	};

	//排序函数
	static double getMidNumOfDim(vector<Point*>& src, int dimension)
	{
		//PointSort::dimension = dimension;
		//sort(src.begin(), src.end(), &PointSort::comp);
		CompareValue compValue (dimension);
		quicksort(src, 0, src.size() - 1, compValue);

		int pos = (src.size() - 1) / 2;
		return src[pos]->getValue(dimension);
	}

	static void setVisited(set<Point*>& visted, Point* p)
	{
		visted.insert(p);
	}

	static bool isVisited(set<Point*>& visited, Point* p)
	{
		return visited.end() != visited.find(p);
	}

	static Point* nearestNeighborImpl(Point* goal, KDTree *tree, set<Point*>& visitedSets)
	{
		KDTree* currentTree = tree;
		Point* currentNearest = currentTree->root;
		int tempDim (-1); //当前维度

		/* 第一步：在kd树中找出包含目标点的叶子结点：从根结点出发，
		* 递归的向下访问kd树，若目标点的当前维的坐标小于切分点的
		* 坐标，则移动到左子结点，否则移动到右子结点，直到子结点为
		* 叶结点为止,以此叶子结点为“当前最近点”
		*/
		while(currentTree != NULL && !currentTree->isLeaf())
		{
			tempDim = currentTree->split;
			if (goal->getValue(tempDim) < currentTree->root->getValue(tempDim))
			{
				if(currentTree->leftChild != NULL)
				{
					currentTree = currentTree->leftChild;
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
					}
				}
			}
			else
			{
				if(currentTree->rightChild != NULL)
				{
					currentTree = currentTree->rightChild;
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
					}
				}
				
			}
		}
		currentNearest = currentTree->root;


		//如果搜索区域对应的子kd树的根结点不是整个kd树的根结点，继续回退搜索
		while (currentTree->parent != NULL)
		{
			//左子树存在，且未访问过则一直向左移动
			if(currentTree->leftChild != NULL 
				&& !isVisited(visitedSets, currentTree->leftChild->root))
			{
				currentTree = currentTree->leftChild;
				continue;
			}
			else
			{

				//若更小，则更新最近点
				if(measureDistance(*goal, *currentTree->root) < measureDistance(*goal, *currentNearest))
				{
					currentNearest = currentTree->root;
				}
				setVisited(visitedSets, currentTree->root); //已经访问

				//搜索区域的维度及该维度下超平面的值
				int searchDim = currentTree->parent->split;
				double superPlaneValue = currentTree->parent->root->getValue(searchDim);
				//目标点到超平面的距离
				double districtDistance = abs(goal->getValue(searchDim) - superPlaneValue);


				KDTree* anotherTree;
				//左右子树切换
				currentTree->isLeft() ? anotherTree = currentTree->parent->rightChild : anotherTree = currentTree->parent->leftChild;
				//如果“目标点到超平面的距离”比“目标点到当前最近点距离”短，表明父节点或
				//另一子树可能存在距离目标点更近的点
				if(districtDistance < measureDistance(*goal, *currentNearest))
				{
					if(anotherTree != NULL && !isVisited(visitedSets, anotherTree->root))
					{
						//若存在或未访问则将当前树切换过去
						currentTree = anotherTree;
						continue;
					}
					else
					{
						currentTree = currentTree->parent;
					}
				}
				else
				{
					if(anotherTree != NULL)
					{
						//另一边子树明显更大，设置为已访问（防止再次迭代）
						setVisited(visitedSets, anotherTree->root); //已经访问
					}
					currentTree = currentTree->parent;
				}
			}
		}
		return currentNearest;
	}

	//删除列表
	vector<KDTree*> delList;
	//从此处创建子树，可以释放内存
	KDTree* generateNewTree()
	{
		KDTree* tree = new KDTree();
		delList.push_back(tree);
		return tree;
	}

	//释放子树的内存
	void freeMemory()
	{
		for(vector<KDTree*>::iterator it = delList.begin(); it != delList.end(); ++it)
		{
			if( *it != NULL)
			{
				delete *it;
				*it = NULL;
			}
		}
		vector<KDTree*>().swap(delList);
	}

	//快速排序
	static void quicksort(vector<Point*>& vec, int low, int high, ICompare& compare)//必须传引用,否则出错,因为vector是一个类对象
	{
		if (low < high)
		{
			int l = low;
			int r = high;
			Point* key = vec[l];//记录key值

			while (l < r)
			{
				while (l < r && compare.comp(key, vec[r]))//从右往左遍历,找到第一个小于key的元素
					--r;
				vec[l] = vec[r];
				while (l < r 
					&& !compare.comp(key, vec[l]))//从左往右遍历,找到第一个大于key值的元素
					++l;
				vec[r] = vec[l];
			}
			vec[l] = key;//其实此时l=r

			quicksort(vec, low, l-1, compare);
			quicksort(vec, r + 1, high, compare);
		}
	}
};
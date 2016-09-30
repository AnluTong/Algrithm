#pragma once

#include <vector>
#include <algorithm>
#include <set>
#include "Point.h"

using namespace std;

class KDTree
{
public: 
	//��ǰֵ
	Point* root;
	//���ڵ�
	KDTree* parent;
	//������
	KDTree* leftChild;
	//������
	KDTree* rightChild;
	//��ǰ����ά��
	int split;

	//Ĭ�Ϲ��캯��
	KDTree(){parent = leftChild = rightChild = NULL;}
	~KDTree(){freeMemory();}

	//�ж�kd���Ƿ�Ϊ��
	bool isEmpty()
	{
		return root == NULL;
	}

	//�ж�kd���Ƿ�ֻ��һ��Ҷ�ӽ��
	bool isLeaf()
	{
		return root != NULL 
			&& rightChild == NULL 
			&& leftChild == NULL;
	}

	//�ж��Ƿ������ĸ����
	bool isRoot()
	{
		return (!isEmpty()) 
			&& parent == NULL;
	}

	//�жϸ���kd���ĸ�����Ƿ����丸kd��������
	bool isLeft()
	{
		if(parent->leftChild != NULL)
		{
			return parent->leftChild->root == root;
		}
		return false;
	}

	//�жϸ���kd���ĸ�����Ƿ����丸kd�����ҽ��
	bool isRight()
	{
		if(parent-rightChild != NULL)
		{
			return parent->rightChild->root == root;
		}
		return false;
	}

public:
	//����kd��
	static void buildKdTree(KDTree* tree, vector<Point*> data, unsigned depth)
	{
		if(tree == NULL)
		{
			return;
		}
		//����������
		unsigned samplesNum = data.size();
		//��ֹ����
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

		//��������ά��
		int k = Point::getDimensionAmount();
		//ѡ���з�ά��
		int splitAttribute = depth % k;
		//��������λ��
		double splitValue = getMidNumOfDim(data, splitAttribute);

		// ����ѡ�����з����Ժ��з�ֵ�������ݼ���Ϊ�����Ӽ�
		vector<Point*> subsetLeft;
		vector<Point*> subsetRight;

		//����λ��С�ķ����󼯺ϣ���ķ����Ҽ���
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

		//�Ӽ��ݹ����buildKdTree����
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

	//��kd��tree������Ŀ���goal�������
	//���룺Ŀ��㣻�ѹ����kd��
	//�����Ŀ���������
	static Point* searchNearestNeighbor(Point* goal, KDTree *tree)
	{
		set<Point*> visitedSets;//Ԫ���Ƿ���ʹ�

		return nearestNeighborImpl(goal, tree, visitedSets);
	}

	//k����������ֱ�Ӵ�set�л�ȡ��������С��k��ֱ�ӷ���set�����е�
	//���ܲ�̫׼ȷ����׼ȷ�ķ�ʽ�ǽ���k�����������
	static vector<Point*> knn(Point* goal, KDTree* tree, unsigned int k)
	{
		set<Point*> visitedSets;//Ԫ���Ƿ���ʹ�
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

	//������
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
		int tempDim (-1); //��ǰά��

		/* ��һ������kd�����ҳ�����Ŀ����Ҷ�ӽ�㣺�Ӹ���������
		* �ݹ�����·���kd������Ŀ���ĵ�ǰά������С���зֵ��
		* ���꣬���ƶ������ӽ�㣬�����ƶ������ӽ�㣬ֱ���ӽ��Ϊ
		* Ҷ���Ϊֹ,�Դ�Ҷ�ӽ��Ϊ����ǰ����㡱
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


		//������������Ӧ����kd���ĸ���㲻������kd���ĸ���㣬������������
		while (currentTree->parent != NULL)
		{
			//���������ڣ���δ���ʹ���һֱ�����ƶ�
			if(currentTree->leftChild != NULL 
				&& !isVisited(visitedSets, currentTree->leftChild->root))
			{
				currentTree = currentTree->leftChild;
				continue;
			}
			else
			{

				//����С������������
				if(measureDistance(*goal, *currentTree->root) < measureDistance(*goal, *currentNearest))
				{
					currentNearest = currentTree->root;
				}
				setVisited(visitedSets, currentTree->root); //�Ѿ�����

				//���������ά�ȼ���ά���³�ƽ���ֵ
				int searchDim = currentTree->parent->split;
				double superPlaneValue = currentTree->parent->root->getValue(searchDim);
				//Ŀ��㵽��ƽ��ľ���
				double districtDistance = abs(goal->getValue(searchDim) - superPlaneValue);


				KDTree* anotherTree;
				//���������л�
				currentTree->isLeft() ? anotherTree = currentTree->parent->rightChild : anotherTree = currentTree->parent->leftChild;
				//�����Ŀ��㵽��ƽ��ľ��롱�ȡ�Ŀ��㵽��ǰ�������롱�̣��������ڵ��
				//��һ�������ܴ��ھ���Ŀ�������ĵ�
				if(districtDistance < measureDistance(*goal, *currentNearest))
				{
					if(anotherTree != NULL && !isVisited(visitedSets, anotherTree->root))
					{
						//�����ڻ�δ�����򽫵�ǰ���л���ȥ
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
						//��һ���������Ը�������Ϊ�ѷ��ʣ���ֹ�ٴε�����
						setVisited(visitedSets, anotherTree->root); //�Ѿ�����
					}
					currentTree = currentTree->parent;
				}
			}
		}
		return currentNearest;
	}

	//ɾ���б�
	vector<KDTree*> delList;
	//�Ӵ˴����������������ͷ��ڴ�
	KDTree* generateNewTree()
	{
		KDTree* tree = new KDTree();
		delList.push_back(tree);
		return tree;
	}

	//�ͷ��������ڴ�
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

	//��������
	static void quicksort(vector<Point*>& vec, int low, int high, ICompare& compare)//���봫����,�������,��Ϊvector��һ�������
	{
		if (low < high)
		{
			int l = low;
			int r = high;
			Point* key = vec[l];//��¼keyֵ

			while (l < r)
			{
				while (l < r && compare.comp(key, vec[r]))//�����������,�ҵ���һ��С��key��Ԫ��
					--r;
				vec[l] = vec[r];
				while (l < r 
					&& !compare.comp(key, vec[l]))//�������ұ���,�ҵ���һ������keyֵ��Ԫ��
					++l;
				vec[r] = vec[l];
			}
			vec[l] = key;//��ʵ��ʱl=r

			quicksort(vec, low, l-1, compare);
			quicksort(vec, r + 1, high, compare);
		}
	}
};
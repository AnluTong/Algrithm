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
	//��ǰֵ
	Point<T>* root;
	//���ڵ�
	KDTree<T>* parent;
	//������
	KDTree<T>* leftChild;
	//������
	KDTree<T>* rightChild;
	//��ǰ����ά��
	int split;

	//Ĭ�Ϲ��캯��
	KDTree(){parent = leftChild = rightChild = NULL;}
	~KDTree(){freeMemory();}

	//�ж�kd���Ƿ�Ϊ��
	bool isEmpty(){ return root == NULL;}
	//�ж�kd���Ƿ�ֻ��һ��Ҷ�ӽ��
	bool isLeaf(){ return root != NULL && rightChild == NULL && leftChild == NULL;}
	//�ж��Ƿ������ĸ����
	bool isRoot(){ return (!isEmpty()) && parent == NULL;}
	//�жϸ���kd���ĸ�����Ƿ����丸kd��������
	bool isLeft(){ if (parent->leftChild != NULL) return parent->leftChild->root == root; return false;}
	//�жϸ���kd���ĸ�����Ƿ����丸kd�����ҽ��
	bool isRight() { if(parent-rightChild != NULL) return parent->rightChild->root == root; return false;}

	//�Ӵ˴����������������ͷ��ڴ�
	KDTree<T>* generateNewTree(){ KDTree<T>* tree = new KDTree<T>(); delList.push_back(tree); return tree;}

private:
	//ɾ���б�
	vector<KDTree<T>*> delList;

	//�ͷ��������ڴ�
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
	//����kd��
	static void buildKdTree(KDTree<T>* tree, vector<Point<T>*> data, unsigned depth)
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
		//��������ά��
		int k = data[0]->getDimensionAmount();

		if (samplesNum == 1)
		{
			tree->root = data[0];
			tree->split = depth % k;
			return;
		}

		//ѡ���з�ά��
		int splitAttribute = depth % k;
		//��������λ��
		double splitValue = getMidNumOfDim(data, splitAttribute);

		// ����ѡ�����з����Ժ��з�ֵ�������ݼ���Ϊ�����Ӽ�
		vector<Point<T>*> subsetLeft;
		vector<Point<T>*> subsetRight;

		//����λ��С�ķ����󼯺ϣ���ķ����Ҽ���
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

	//k�������������ƣ������������ѣ�һ�����ʽ��ѣ�һ����ѡ·����
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
		}

		vector<Point<T>*> result = routerQueue.allData();

		ATSort<Point<T>*>::sort(result, routerComp);
		return result;
	}

private:
	//��������ĳά�ȵ�����ֵ��������
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

	//�Է��ʽ��Ķѽ�������
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

	//�Ժ�ѡ·���ѽ�������
	class CompareCandicatePoint : public ATICompare<KDTree<T>*>
	{
	public:
		CompareCandicatePoint(Point<T>* dest) {sortGoal = dest;}

		virtual bool compare(KDTree<T>*& left, KDTree<T>*& right)
		{
			/**Ŀ����볬ƽ�������Ϊ��ѡ�����ȼ������׼**/
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

	//�����У�����ĳά�ȵ���λ��
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

	//����֪λ��������Ҷ�ӽ��
	static KDTree<T>* searchLeafWithHeap(Point<T>* goal, KDTree<T> *tree, int k, ATPriorityQueue<Point<T>*>& router, ATPriorityQueue<KDTree<T>*>& candicater)
	{
		KDTree<T>* currentTree = tree;
		router.add(currentTree->root); //·�����

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
						candicater.add(currentTree->rightChild);//��һ��֧��Ϊ��ѡ�����
					}

					currentTree = currentTree->leftChild;
					router.add(currentTree->root); //·�����
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
						router.add(currentTree->root); //·�����
					}
				}
			}
			else
			{
				if(currentTree->rightChild != NULL)
				{
					if(currentTree->leftChild != NULL)
					{
						candicater.add(currentTree->leftChild); //��һ��֧��Ϊ��ѡ�����
					}

					currentTree = currentTree->rightChild;
					router.add(currentTree->root);//·�����
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
						router.add(currentTree->root);//·�����
					}
				}
				
			}
		}

		return currentTree;
	}

	//ԭʼ������������㷨����BBF
	//static Point<T>* nearestNeighborImpl(Point<T>* goal, KDTree<T> *tree, set<Point<T>*>& visitedSets)
	//{
	//	Point<T>* currentNearest = tree->root;
	//	KDTree<T>* currentTree = searchLeaf(goal, tree);
	//	currentNearest = currentTree->root;


	//	//������������Ӧ����kd���ĸ���㲻������kd���ĸ���㣬������������
	//	while (currentTree->parent != NULL)
	//	{
	//		//���������ڣ���δ���ʹ���һֱ�����ƶ�
	//		if(currentTree->leftChild != NULL 
	//			&& !isVisited(visitedSets, currentTree->leftChild->root))
	//		{
	//			currentTree = currentTree->leftChild;
	//			continue;
	//		}
	//		else
	//		{

	//			//����С������������
	//			if(measureDistance(*goal, *currentTree->root) < measureDistance(*goal, *currentNearest))
	//			{
	//				currentNearest = currentTree->root;
	//			}
	//			setVisited(visitedSets, currentTree->root); //�Ѿ�����

	//			//���������ά�ȼ���ά���³�ƽ���ֵ
	//			int searchDim = currentTree->parent->split;
	//			double superPlaneValue = currentTree->parent->root->getValue(searchDim);
	//			//Ŀ��㵽��ƽ��ľ���
	//			double districtDistance = abs(goal->getValue(searchDim) - superPlaneValue);


	//			KDTree<T>* anotherTree;
	//			//���������л�
	//			currentTree->isLeft() ? anotherTree = currentTree->parent->rightChild : anotherTree = currentTree->parent->leftChild;
	//			//�����Ŀ��㵽��ƽ��ľ��롱�ȡ�Ŀ��㵽��ǰ�������롱�̣��������ڵ��
	//			//��һ�������ܴ��ھ���Ŀ�������ĵ�
	//			if(districtDistance < measureDistance(*goal, *currentNearest))
	//			{
	//				if(anotherTree != NULL && !isVisited(visitedSets, anotherTree->root))
	//				{
	//					//�����ڻ�δ�����򽫵�ǰ���л���ȥ
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
	//					//��һ���������Ը�������Ϊ�ѷ��ʣ���ֹ�ٴε�����
	//					setVisited(visitedSets, anotherTree->root); //�Ѿ�����
	//				}
	//				currentTree = currentTree->parent;
	//			}
	//		}
	//	}
	//	return currentNearest;
	//}
};
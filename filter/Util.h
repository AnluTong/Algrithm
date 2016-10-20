#pragma once
#include <string>
#include <sys/timeb.h>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <hash_set>

using namespace std;

#define SHOW 1

class Util
{
public:
	static Util& getInstance();
	~Util();
	long getCurrentTime();
	Util& log(char* c);
	Util& log(int i);
	Util& log(string s);
	Util& log(double d);
	Util& endl();

private:
	Util();
	//把复制构造函数和=操作符也设为私有,防止被复制
	Util(const Util&);
	Util& operator=(const Util&);

	static Util* instance;
};
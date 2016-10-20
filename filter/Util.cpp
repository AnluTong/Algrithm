#include "stdafx.h"
#include "Util.h"


Util::Util()
{
}


Util::Util(const Util&)
{
}


Util& Util::operator=(const Util&)
{
	return *this;
}

//在此处初始化
Util* Util::instance = new Util();

Util& Util::getInstance()
{
	return *instance;
}

Util::~Util()
{
	if(instance != NULL)
	{
		delete instance;
	}
	instance = NULL;
}

long Util::getCurrentTime()
{    
	struct timeb t1;  
	ftime(&t1);  
	return t1.time;
}

Util& Util::log(char* c)
{
	if(SHOW)
		printf("%s", c); 
	return *this;
}

Util& Util::log(int i)
{
	if(SHOW)
		printf("%d", i); 
	return *this;
}

Util& Util::log(string s)
{
	if(SHOW)
		printf("%s", s.c_str()); 
	return *this;
}

Util& Util::log(double d)
{
	if(SHOW)
		printf("%5f", d); 
	return *this;
}

Util& Util::endl()
{
	if(SHOW)
		printf("\n"); 
	return *this;
}
#pragma once

template <class T>
class ATICompare
{
public:
	/**��С���ң���Ϊtrue**/
	virtual bool compare(T& left, T& right) = 0;
};
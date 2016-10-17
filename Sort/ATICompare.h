#pragma once

template <class T>
class ATICompare
{
public:
	/**左小于右，则为true**/
	virtual bool compare(T& left, T& right) = 0;
};
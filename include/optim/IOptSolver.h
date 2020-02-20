#ifndef __IOPTSOLVER
#define __IOPTSOLVER
#include <valarray>
#include "OptimTask.h"


class IMinimizer1{
public:
	virtual void solve(const MinTask1 &) = 0;
	virtual void getSolution(s_type &)const = 0;
	virtual void getBestVal(s_type &)const = 0;
	virtual void setParam(const std::valarray<double>&) = 0;
	virtual void setParam(const std::valarray<int>&) = 0;
	virtual void getParam(std::valarray<double>&)const = 0;
	virtual void getParam(std::valarray<int>&)const = 0;
};

class IMinimizerN{
public:
	virtual void solve(const MinTaskN &) = 0;
	virtual void getSolution(v_type &)const = 0;
	virtual void getBestVal(s_type &)const = 0;
	virtual void setParam(const std::valarray<double>&) = 0;
	virtual void setParam(const std::valarray<int>&) = 0;
	virtual void getParam(std::valarray<double>&)const = 0;
	virtual void getParam(std::valarray<int>&)const = 0;
};

class ISolver{
public:
	virtual void solve(const SolveTask &) = 0;
	virtual void getSolution(v_type &)const = 0;
	virtual void getBestVal(s_type &)const = 0;
	virtual void setParam(const std::valarray<double>&) = 0;
	virtual void setParam(const std::valarray<int>&) = 0;
	virtual void getParam(std::valarray<double>&)const = 0;
	virtual void getParam(std::valarray<int>&)const = 0;
};

#endif

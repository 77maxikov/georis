#ifndef __IOPTSOLVER
#define __IOPTSOLVER
#include <valarray>
#include "OptimTask.h"


class ISolver{
public:
    virtual int solve(const SolveTask &) = 0;
	virtual void getSolution(v_type &)const = 0;
	virtual void getBestVal(s_type &)const = 0;
    virtual void setParam(const std::valarray<double>&) = 0;
    virtual void setParam(const std::valarray<int>&) = 0;
    virtual void getParam(std::valarray<double>&)const = 0;
    virtual void getParam(std::valarray<int>&)const = 0;
};

#endif

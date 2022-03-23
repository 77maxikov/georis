#ifndef _SOLVERNREG_H
#define _SOLVERNREG_H
#include "IOptSolver.h"
// Newton with regularization NLS solver
class SolverNReg:public ISolver{
public:
    SolverNReg();

    virtual int solve(const SolveTask &task);
    virtual void getSolution(v_type &best)const{best = _bestx;};
    virtual void getBestVal(s_type &)const{};

    virtual void setParam(const std::valarray<double>&parm){};
    virtual void setParam(const std::valarray<int>&){};
    virtual void getParam(std::valarray<double>&v)const{};
    virtual void getParam(std::valarray<int>&)const{};

protected:
	unsigned _fevals;
	v_type _bestx;
};

#endif // _SolverNReg_H

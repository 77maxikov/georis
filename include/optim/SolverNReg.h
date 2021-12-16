#ifndef _SOLVERNREG_H
#define _SOLVERNREG_H
#include "IOptSolver.h"
// Newton with regularization NLS solver
class SolverNReg:public ISolver{
public:
    SolverNReg();

	void solve(const SolveTask &task);
	void getSolution(v_type &best)const{best = _bestx;};
	void getBestVal(s_type &)const{};

	void setParam(const std::valarray<double>&parm){};
	void setParam(const std::valarray<int>&){};
	void getParam(std::valarray<double>&v)const{};
	void getParam(std::valarray<int>&)const{};

protected:
	unsigned _fevals;
	v_type _bestx;
};

#endif // _SolverNReg_H

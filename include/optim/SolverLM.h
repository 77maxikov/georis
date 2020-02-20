#ifndef _SOLVERLM_H
#define _SOLVERLM_H
#include "IOptSolver.h"
// LevenbergMarquardt NLS solver
class SolverLM:public ISolver{
public:
	SolverLM();

	void solve(const SolveTask &task);
	void getSolution(v_type &best)const{best = _bestx;};
	void getBestVal(s_type &)const{};

	void setParam(const std::valarray<double>&parm);
	void setParam(const std::valarray<int>&){};
	void getParam(std::valarray<double>&v)const{};
	void getParam(std::valarray<int>&)const{};

protected:
	unsigned _fevals;
	double _mu;
	double _mu_inc;
	double _mu_dec;
	v_type _bestx;
};

#endif // _SolverLM_H

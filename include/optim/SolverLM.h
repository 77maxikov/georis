#ifndef _SOLVERLM_H
#define _SOLVERLM_H
#include "IOptSolver.h"
// LevenbergMarquardt NLS solver
class SolverLM:public ISolver{
public:
	SolverLM();

    virtual int solve(const SolveTask &task);
    virtual void getSolution(v_type &best)const{best = _bestx;};
    virtual void getBestVal(s_type &)const{};

    virtual void setparamProxy(const std::valarray<double>&parm);
    virtual void setparamProxy(const std::valarray<int>&){};
    virtual void getparamProxy(std::valarray<double>&v)const{};
    virtual void getparamProxy(std::valarray<int>&)const{};

protected:
	unsigned _fevals;
	double _mu;
	double _mu_inc;
	double _mu_dec;
	v_type _bestx;
};

#endif // _SolverLM_H

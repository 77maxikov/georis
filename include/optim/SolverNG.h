#ifndef _SOLVERNG_H
#define _SOLVERNG_H
#include "IOptSolver.h"
// Newton - Gauss NLS solver
class SolverNG:public ISolver{
public:
	SolverNG();

    virtual int solve(const SolveTask &task);
    virtual void getSolution(v_type &best)const{best = _bestx;};
    virtual void getBestVal(s_type &)const{};

    virtual void setparamProxy(const std::valarray<double>&parm){};
    virtual void setparamProxy(const std::valarray<int>&){};
    virtual void getparamProxy(std::valarray<double>&v)const{};
    virtual void getparamProxy(std::valarray<int>&)const{};

protected:
	unsigned _fevals;
	v_type _bestx;
};

#endif // _SolverNG_H

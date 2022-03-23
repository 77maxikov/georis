#ifndef SOLVERCERES_H
#define SOLVERCERES_H
#include "IOptSolver.h"

class SolverCeres:public ISolver {
public:
    SolverCeres();
    virtual int solve(const SolveTask &task);
    virtual void getSolution(v_type &best)const {
        best = _bestx;
    };
    virtual void getBestVal(s_type &)const {};

    virtual void setParam(const std::valarray<double>&parm);
    virtual void setParam(const std::valarray<int>&) {};
    virtual void getParam(std::valarray<double>&v)const {};
    virtual void getParam(std::valarray<int>&)const {};

protected:
private:
	 v_type _bestx;
};

#endif // SOLVERCERES_H

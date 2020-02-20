#ifndef SOLVERCERES_H
#define SOLVERCERES_H
#include "IOptSolver.h"

class SolverCeres:public ISolver {
public:
    SolverCeres();
    void solve(const SolveTask &task);
    void getSolution(v_type &best)const {
        best = _bestx;
    };
    void getBestVal(s_type &)const {};

    void setParam(const std::valarray<double>&parm);
    void setParam(const std::valarray<int>&) {};
    void getParam(std::valarray<double>&v)const {};
    void getParam(std::valarray<int>&)const {};

protected:
private:
	 v_type _bestx;
};

#endif // SOLVERCERES_H

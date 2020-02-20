#ifndef _OPTSOLVERNEWTON_H
#define _OPTSOLVERNEWTON_H
#include "IOptSolver.h"

class OptSolverNewton:public IMinimizerN{
public:
    OptSolverNewton();
    ~OptSolverNewton();
    // IOptSolver
    void solve(const MinTaskN &task);
    void getSolution(v_type &)const;
    void getBestVal(s_type &)const {};

    void setParam(const std::valarray<double>&);
    void setParam(const std::valarray<int>&);
    void getParam(std::valarray<double>&)const {};
    void getParam(std::valarray<int>&)const;
private:
    unsigned _maxIterCount,_IterCount,_maxFunCalls,_FunCalls;
    v_type _X;
};

#endif // _OPTSOLVERNEWTON_H

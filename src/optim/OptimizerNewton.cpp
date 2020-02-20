#include "OptSolverNewton.h"
#include <stdexcept>

OptSolverNewton::OptSolverNewton(){
  	_maxIterCount = 1000;
	_IterCount = 0;
	_maxFunCalls = 1000;
	_FunCalls = 0;

}

OptSolverNewton::~OptSolverNewton()
{
    //dtor
}
void OptSolverNewton::setParam(const std::valarray<double>&){
}
void OptSolverNewton::setParam(const std::valarray<int>&){
}
void OptSolverNewton::solve(const MinTaskN &task){
    if (!task.target->hasGrad())
        throw std::invalid_argument("OptSolverNewton: Target should have gradient!");
    if ( !task.target->hasHess() )
        throw std::invalid_argument("OptSolverNewton: Target should have hessian!");
    _X = task.x0;
    s_type f_prev = (*task.target)(_X);
    while (_FunCalls < _maxFunCalls && _IterCount < _maxIterCount){
        _X = _X - (task.target->getHess(_X)).inverse()*task.target->getGrad(_X);
        s_type f = (*task.target)(_X);
        ++_FunCalls;
        ++_IterCount;
        if (f_prev > f) break;
        f_prev = f;
    }
}
void OptSolverNewton::getSolution(v_type &x)const{
    x = _X;
};
void OptSolverNewton::getParam(std::valarray<int> &parmi)const{
    parmi.resize(2);
	parmi[0] = _IterCount;
    parmi[1] = _FunCalls;
}

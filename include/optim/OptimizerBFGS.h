#ifndef _OPTSOLVERBFGS_H
#define _OPTSOLVERBFGS_H
#include "IOptSolver.h"

class OptSolverBFGS:public IMinimizerN{
	   unsigned _fevals;
		v_type _bestx;
	public:
		OptSolverBFGS();
		void solve(const MinTaskN &task);
		void getSolution(v_type &)const{};
		void getBestVal(s_type &)const{};
		void setParam(const std::valarray<double>&){};
		void setParam(const std::valarray<int>&){};
		void getParam(std::valarray<double>&)const{};
		void getParam(std::valarray<int>&)const{};
};

#endif // _OPTSOLVERBFGS_H

#ifndef _OPTSOLVERNM_H
#define _OPTSOLVERNM_H
#include "IOptSolver.h"
// Nelder-Mead optimizer
class OptSolverNM:public IMinimizerN{
public:
	OptSolverNM();
	~OptSolverNM();

	// IOptSolver
	void solve(const MinTaskN &task);
	void getSolution(v_type &)const;
	void getBestVal(s_type &)const{};
	void setParam(const std::valarray<double>&);
	void setParam(const std::valarray<int>&);
	void getParam(std::valarray<double>&)const{};
	void getParam(std::valarray<int>&)const;

protected:
	bool _optsSet;
	unsigned _maxIterCount,_IterCount,_maxFunCalls,_FunCalls;
	double _rho,_chi,_psi,_sigma,_tolf,_tolx;
	v_type _X;
};
#endif

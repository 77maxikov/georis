#ifndef _OPTIMTASK_H
#define _OPTIMTASK_H
#include "IOptFunc.h"

#define OPTFLAG_TOLX 0x1
#define OPTFLAG_TOLF 0x2
#define OPTFLAG_FEVALS 0x4

struct StopCondition{
	s_type tolx;
	s_type tolf;
	unsigned fevals;
	StopCondition(double tx = 1e-6,
					  double tf = 1e-6,
					  unsigned fe = 1000):tolx(tx),tolf(tf),fevals(fe){};
};

template<class func,class arg = v_type> struct OptimTaskBase{
	func		*target; // target f(x)
	arg		x0; // initial
	arg		*lb,*ub; // lower and upper bounds
	StopCondition stopcond;
	OptimTaskBase(){target = 0;lb = ub = 0;}
};

// General minimization task for scalar function of scalar argument
typedef OptimTaskBase<IOptFunc1_1,s_type> MinTask1;
// General minimization task for scalar function of vector argument
typedef OptimTaskBase<IOptFunc1_N,v_type> MinTaskN;

// General equation solution task for scalar function of scalar argument
typedef OptimTaskBase<IOptFunc1_1,s_type> SolveTask1;
// General equation solution task for scalar function of vector argument
typedef OptimTaskBase<IOptFunc1_N,v_type> SolveTaskN;
// General equation solution task for vector function of vector argument
typedef OptimTaskBase<IOptFuncN,v_type> SolveTask;

#endif // _OPTIMTASK_H

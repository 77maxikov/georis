#include "OptSolverGD.h"
#include <stdexcept>
#include <iostream>
#include "line_search.h"


OptSolverGD::OptSolverGD() {
    _fevals = 0;
}

void OptSolverGD::solve(const MinTaskN &task) {
    if (!(*task.target).hasGrad()) throw std::invalid_argument("OptSolverGD::target function should provide gradient");
    v_type x1(task.x0);
    _bestx = x1;
    unsigned flag = 0;
    while (1) {
        if (_fevals > task.stopcond.fevals) {
				flag |= OPTFLAG_FEVALS;
				break;
        }
        s_type fx1 = (*task.target)(x1); ++_fevals;
        std::cout << "OptSolverGD::solve - fx1  = " << fx1 <<std::endl;
        v_type antigrad = -task.target->getGrad(x1);
        std::cout << "OptSolverGD::solve - dir = "<< std::endl<< antigrad << std::endl << "grad_norm = " << norm2(antigrad) << std::endl;
			lineSearchGolden(antigrad,task,x1,_fevals);
			//lineSearchGolden(-antigrad,task,x1,_fevals);
			if (norm2(_bestx - x1) < task.stopcond.tolx) {
				flag |= OPTFLAG_TOLX;
				break;
			}
        _bestx = x1;
	 }
	 std::cout << "OptSolverGD::solve - flag = " << flag << " fevals " << _fevals << std::endl;
    (*task.target)(_bestx);

}

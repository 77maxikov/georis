#include "OptSolverBFGS.h"
#include <stdexcept>
#include "line_search.h"
#include <iostream>

OptSolverBFGS::OptSolverBFGS() {
    _fevals = 0;
}
void OptSolverBFGS::solve(const MinTaskN &task) {
    if (!(*task.target).hasGrad()) throw std::invalid_argument("OptSolverBFGS::target function should provide gradient");
std::cout << "task.x0.size() = " << task.x0.size() <<std::endl;

/*    m_type H(task.x0.size(),task.x0.size()) ;
    H.setZero();
    for (int k = 0;k < task.x0.size();++k){
		H(k,k) = 1;
    }

    v_type x1(task.x0);
    s_type fx1 = (*task.target)(x1);
    v_type antigrad = -task.target->getGrad(x1);
std::cout << "antigrad  = " << antigrad  << std::endl;
    _bestx = x1;
    unsigned flag = 0;
    while (1) {
        if (_fevals > task.stopcond.fevals) {
            flag |= OPTFLAG_FEVALS;
            break;
        }
        ++_fevals;
std::cout << H <<std::endl;
std::cout << "==========================" <<std::endl;
std::cout << H.inverse() <<std::endl;
		  v_type dir = H.inverse()*antigrad;
std::cout << "New direction = " << dir << std::endl;
		  v_type x2(x1);
//std::cin.get();
        lineSearchGolden(dir,task,x2,_fevals);
        if (norm(_bestx - x2) < task.stopcond.tolx) {
            flag |= OPTFLAG_TOLX;
            break;
        }
        s_type fx2 = (*task.target)(x2);
std::cout << "New function val = " << fx2 << std::endl;
		  if ( fx2 > fx1 || fx1 < fx2 + task.stopcond.tolf ){
            flag |= OPTFLAG_TOLF;
            break;
        }
        _bestx = x2;
        // Hessian aproximation update
        v_type s(x2 - x1);
        v_type grad_new = task.target->getGrad(x2);
        v_type y = grad_new  + antigrad;
		  H = H + y*y.transpose()/(y.transpose()*s) - H*s*s.transpose()*H/(s.transpose()*H*s);
		  antigrad = -grad_new;
		  x1 = x2;
        std::cout << "-----------------------------"         <<std::endl;
    }
*/

    m_type invH(task.x0.size(),task.x0.size()) ;
    invH.setZero();
    for (int k = 0;k < task.x0.size();++k){
		invH(k,k) = 1;
    }

    v_type x1(task.x0);
    s_type fx1 = (*task.target)(x1);++_fevals;
    v_type antigrad = -task.target->getGrad(x1);
std::cout << "antigrad  = "<<std::endl << antigrad  << std::endl;
    _bestx = x1;
    unsigned flag = 0;
    while (1) {
        if (_fevals > task.stopcond.fevals) {
            flag |= OPTFLAG_FEVALS;
            break;
        }
//std::cout << invH <<std::endl;
std::cout << "==========================" <<std::endl;
		  v_type dir = invH*antigrad;
std::cout << "New direction = " << std::endl << dir << std::endl;
		  v_type x2(x1);
        lineSearchGolden(dir,task,x2,_fevals);
        if (norm2(_bestx - x2) < task.stopcond.tolx) {
            flag |= OPTFLAG_TOLX;
            break;
        }
        s_type fx2 = (*task.target)(x2);
std::cout << "New function val = " << fx2 << std::endl;
		  if ( fx2 > fx1 || fx1 < fx2 + task.stopcond.tolf ){
            flag |= OPTFLAG_TOLF;
            break;
        }
        _bestx = x2;
        // Hessian inversion aproximation update
        v_type s(x2 - x1);
        v_type grad_new = task.target->getGrad(x2);
        v_type y = grad_new  + antigrad;
        s_type den = s.transpose()*y;

std::cout << "Den = " << den <<std::endl;
std::cout << "First " << (s.transpose()*y) <<std::endl<< (y.transpose()*invH*y)<<std::endl<< (s*s.transpose()) << std::endl;
std::cout << "Second " << (invH*y*s.transpose()) <<std::endl<<"----"<<std::endl<< (s*y.transpose()*invH) << std::endl;
std::cout << "Third " << (1/den/den*(s.transpose()*y +y.transpose()*invH*y))<<std::endl<<std::endl << (s*s.transpose()) << std::endl;
std::cout << "Fourth " << 1/den*(invH*y*s.transpose() + s*y.transpose()*invH) << std::endl;

		  invH = invH + 1/den/den*(den + y.transpose()*invH*y)*(s*s.transpose()) - 1/den*(invH*y*s.transpose() + s*y.transpose()*invH);
		  antigrad = -grad_new;
		  x1 = x2;
        std::cout << "-----------------------------"         <<std::endl;
    }
    std::cout << "flag = " << flag << " fevals " << _fevals << std::endl;
    (*task.target)(_bestx);
}

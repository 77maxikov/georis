#include "SolverNG.h"
#include <Eigen/Eigenvalues>
#include <stdexcept>
#include <iostream>

SolverNG::SolverNG() {
    _fevals = 0;
}
void SolverNG::solve(const SolveTask &task) {
    if (!(*task.target).hasJacob()) throw std::invalid_argument("SolverNG::target function should provide Jacobian");
    v_type x1(task.x0);
    _bestx = x1;
    v_type fx1 = (*task.target)(x1);
    ++_fevals;
    unsigned flag = 0;
    v_type dx(x1.size());

    while (1) {
        if (_fevals > task.stopcond.fevals) {
            flag |= OPTFLAG_FEVALS;
            break;
        }

//std::cout << "SolverNG::solve - x = "<< std::endl << x1 << std::endl;
//std::cout << "SolverNG::solve - fx1  = " << fx1 <<", norm = "<< fx1.norm()<<std::endl;

        m_type jacob = task.target->getJacob(x1);
std::cout << "Jacob:" << std::endl << jacob << std::endl;
        m_type nf = -jacob.transpose()*fx1;
        m_type H = jacob.transpose()*jacob;
        Eigen::EigenSolver<m_type> sol(H);
        std::cout << "The eigenvalues of H are:" << std::endl << sol.eigenvalues() << std::endl;
        std::cout << "Det of H is:" << std::endl << H.determinant() << std::endl;

		dx = H.fullPivLu().solve(nf);
        if ( dx.norm() < task.stopcond.tolx ) {
            flag |= OPTFLAG_TOLX;
            break;
        }

        v_type x2(x1 + dx);
        v_type fx2 = (*task.target)(x2);
        ++_fevals;
        if ( (fx1-fx2).norm() < task.stopcond.tolf ) {
            flag |= OPTFLAG_TOLF;
            break;
        }

        if ( fx2.norm() < fx1.norm() ) {
            x1 = x2;
            fx1 = fx2;
            _bestx = x1;
        }
        else
			break;
    }
std::cout << "SolverNG::solve - finished with flag = " << flag << " fevals " << _fevals << std::endl;
(*task.target)(_bestx);
}


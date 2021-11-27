#include "SolverNG.h"
#include <Eigen/Eigenvalues>
#include <stdexcept>
#include "mooLog.h"

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
MOOLOG  << "Jacob:" << std::endl << jacob << std::endl;
        if (jacob.rows() < jacob.cols() ){
        //if (0){
            v_type dx1 = jacob.colPivHouseholderQr().solve(-fx1);



            m_type H = jacob*jacob.transpose();
            m_type sol = H.colPivHouseholderQr().solve(-fx1);
            v_type dx2 = jacob.transpose()*sol;

            //MOOLOG << "dx1-dx2" << std::endl << (dx1-dx2) << std::endl;
            MOOLOG << "dx1.norm = " << dx1.norm() << std::endl;
            //MOOLOG << "jacob*dx1 + fx " << std::endl << (jacob*dx1 + fx1) << std::endl;

            MOOLOG << "dx2.norm = " << dx2.norm() << std::endl;
            //MOOLOG << "jacob*dx2 + fx " << std::endl << (jacob*dx2 + fx1) << std::endl;

            dx = dx1;
        }
        else{
            m_type nf = -jacob.transpose()*fx1;
            m_type H = jacob.transpose()*jacob;
            Eigen::EigenSolver<m_type> sol(H);
            MOOLOG  << "The eigenvalues of H are:" << std::endl << sol.eigenvalues() << std::endl;
            MOOLOG << "Det of H is:" << std::endl << H.determinant() << std::endl;

            dx = H.fullPivLu().solve(nf);
        }
        MOOLOG << "dx = "<< std::endl << dx << std::endl;
        //MOOLOG << "J*dx " << std::endl << (jacob*dx) << std::endl;
        MOOLOG << "f(x)+J*dx "<<std::endl << (fx1 + jacob * dx) << std::endl;
        MOOLOG << "fx1.transpose()*jacob*dx" << std::endl << fx1.transpose()*jacob*dx << std::endl;
        if ( dx.norm() < task.stopcond.tolx ) {
            flag |= OPTFLAG_TOLX;
            MOOLOG << "TOLX" << std::endl;
            break;
        }

        v_type x2(x1 + dx);
        v_type fx2 = (*task.target)(x2);
        MOOLOG << "fx2 "<< std::endl << fx2 << std::endl;
        ++_fevals;
        if ( (fx1-fx2).norm() < task.stopcond.tolf ) {
            flag |= OPTFLAG_TOLF;
            MOOLOG << "TOLF" << std::endl;
            break;
        }

        if ( fx2.norm() < fx1.norm() ) {
            x1 = x2;
            fx1 = fx2;
            _bestx = x1;
        }
        else{
            MOOLOG << "Calc step size "<<std::endl;
           bool found = false;
           double bestnorm = fx1.norm();
            for (double alpha = 1;alpha >0; alpha -= 0.01){
                v_type x21 = x1 + alpha*dx;
                v_type fx21 = (*task.target)(x21);
                ++_fevals;
                if ( fx21.norm() < bestnorm ){
                    fx2 = fx21;
                    x2 = x21;
                    bestnorm  = fx21.norm();
                    found = true;
                    MOOLOG << " alpha " << alpha << std::endl;
                    //break;
                }
            }
            if ( !found )
                break;
            x1 = x2;
            fx1 = fx2;
            _bestx = x1;
        }
        MOOLOG << "bestx " << std::endl << _bestx.transpose() << std::endl;
    }
MOOLOG << "SolverNG::solve - finished with flag = " << flag << " fevals " << _fevals << std::endl;
(*task.target)(_bestx);
}


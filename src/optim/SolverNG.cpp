#include "SolverNG.h"
#include <Eigen/SparseQR>
#include <stdexcept>

#include "mooLog.h"

SolverNG::SolverNG() {
    _fevals = 0;
}
int SolverNG::solve(const SolveTask &task) {
    if (!(*task.target).hasJacob()) throw std::invalid_argument("SolverNG::target function should provide Jacobian");
    v_type x1(task.x0);
    _bestx = x1;
    v_type fx1 = (*task.target)(x1);++_fevals;
    MOOLOG << "SolverNG::solve - initial values: " << std::endl << fx1 << std::endl<< "err.norm = " << fx1.norm() << std::endl;
    unsigned flag = 0;
    v_type dx(x1.size());
    size_t iter = 0;
    while (1) {
        if (_fevals > task.stopcond.fevals) {
            flag |= OPTFLAG_FEVALS;
            break;
        }
        spam_type jacob = task.target->getJacob(x1);
        //MOOLOG  << "SolverNG::solve - Jacob:" << std::endl << jacob << std::endl;
        if ( jacob.rows() < jacob.cols() ){
            spam_type H = jacob*jacob.transpose();
            Eigen::SparseQR<spam_type,Eigen::COLAMDOrdering<int> > solver(H);
            dx = jacob.transpose()*solver.solve(-fx1);
        }
        else{
            spam_type H = jacob.transpose()*jacob;
            H.makeCompressed();

            Eigen::SparseQR<spam_type,Eigen::COLAMDOrdering<int> > solver(H);
            dx = solver.solve(-jacob.transpose()*fx1);         // use the factorization to solve for the given right hand side
            //MOOLOG << "Det of H is:" << std::endl << H.determinant() << std::endl;
        }
        //MOOLOG << "SolverNG::solve dx = "<< std::endl << dx.transpose() << std::endl;
        //MOOLOG << "f(x)+J*dx "<<std::endl << (fx1 + jacob * dx) << std::endl;
        //MOOLOG << "fx1.transpose()*jacob*dx " << fx1.transpose()*jacob*dx << std::endl;
        if ( dx.norm() < task.stopcond.tolx ) {
            flag |= OPTFLAG_TOLX;
            MOOLOG << "SolverNG::solve TOLX, dx.norm() = " << dx.norm() << std::endl;
            break;
        }

        v_type x2(x1 + dx);
        v_type fx2 = (*task.target)(x2);++_fevals;
        //MOOLOG << "SolverNG::solve fx2 "<< std::endl << fx2 << std::endl;

        if ( abs(fx1.norm() - fx2.norm()) < task.stopcond.tolf ) {
            flag |= OPTFLAG_TOLF;
            MOOLOG << "SolverNG::solve TOLF" << std::endl;
            break;
        }

        if ( fx2.norm() < fx1.norm() ) {
            x1 = x2;
            fx1 = fx2;
            _bestx = x1;
        }
        else{
            //MOOLOG << "Line search "<<std::endl;
            bool found = false;
            size_t nLinsearchIter = 0;
            double bestnorm = fx1.norm();
            double alpha_min = 0.0, alpha_max = 1.0;
            double alpha = 0.5*(alpha_max + alpha_min);
            while ( nLinsearchIter < 100 && !found ){
                v_type x21 = x1 + alpha*dx;
                v_type fx21 = (*task.target)(x21);
                ++_fevals;
                if ( fx21.norm() < bestnorm ){
                    found = true;
                    fx2 = fx21;
                    x2 = x21;
                    bestnorm  = fx21.norm();
                }
                else{
                    alpha_max = alpha;
                }
                alpha = 0.5*(alpha_max + alpha_min);
                ++nLinsearchIter;
            }
            if ( !found ) {
                //MOOLOG << " not found" << std::endl;
                break;
            }
            else{
                //MOOLOG << "alpha = " << alpha << std::endl;
            }
            x1 = x2;
            fx1 = fx2;
            _bestx = x1;
        }
        //MOOLOG << "SolverNG::solve: bestx " << std::endl << _bestx.transpose() << std::endl;
        MOOLOG  << "SolverNG::solve: iter = " << iter <<  " err.norm = " << fx1.norm() << std::endl;
        ++iter;

    }
    MOOLOG << "SolverNG::solve - finished with flag = " << flag << " fevals " << _fevals << std::endl;
    //(*task.target)(_bestx);
    return 0;
}


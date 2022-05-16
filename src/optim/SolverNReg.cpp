#include "SolverNReg.h"
#include <Eigen/Eigenvalues>
#include <stdexcept>
#include "scaler.h"

#include "mooLog.h"

SolverNReg::SolverNReg() {
    m_fevals = 0;
}
int SolverNReg::solve(const SolveTask &task) {
    if (!(*task.target).hasJacob()) throw std::invalid_argument("SolverNReg::target function should provide Jacobian");
    v_type x1(task.x0);
    m_bestx = x1;
    v_type fx1 = (*task.target)(x1);++m_fevals;
//    MOOLOG << "SolverNReg::solve - initial values: " << std::endl << fx1 << std::endl;
    unsigned flag = 0;
    v_type dx(x1.size());
    size_t iter = 0;
    const double Hparam = 0.5;
bool flag1 = true;
    while (1) {
        if ( m_fevals > task.stopcond.fevals ) {
            flag |= OPTFLAG_FEVALS;
            break;
        }

//std::cout << "SolverNReg::solve - x = "<< std::endl << x1 << std::endl;
//std::cout << "SolverNReg::solve - fx1  = " << fx1 <<", norm = "<< fx1.norm()<<std::endl;

        m_type jacob = task.target->getJacob(x1);
//MOOLOG  << "SolverNReg::solve - Jacob:" << std::endl << jacob << std::endl;
        double lambda = sqrt(Hparam*(jacob.transpose()*fx1).norm());

        m_type H = jacob.transpose()*jacob;
        if ( flag1 ){ // Count zero elements
            size_t nzeros = 0;
            for (int r = 0; r < H.rows();++r)
                for (int c = 0; c < H.cols(); ++c ){
                    if ( abs(H(r,c)) < 1e-12 )
                        ++nzeros;
                }
            MOOLOG << "SolverNReg::solve - sparsity: " << static_cast<double>(nzeros) / H.rows() / H.cols() << std::endl;
            flag1 = false;
        }
        for (Eigen::Index k = 0;k< H.cols();++k)
            H(k,k) += lambda;

        dx = H.fullPivLu().solve(-jacob.transpose()*fx1);

//        MOOLOG << "dx = "<< std::endl << dx.transpose() << std::endl;

        //MOOLOG << "f(x)+J*dx "<<std::endl << (fx1 + jacob * dx) << std::endl;
        //MOOLOG << "fx1.transpose()*jacob*dx " << fx1.transpose()*jacob*dx << std::endl;
        if ( dx.norm() < task.stopcond.tolx ) {
            flag |= OPTFLAG_TOLX;
            MOOLOG << "TOLX" << std::endl;
            break;
        }

        v_type x2(x1 + dx);
        v_type fx2 = (*task.target)(x2);++m_fevals;
//        MOOLOG << "fx2 "<< std::endl << fx2 << std::endl;

        if ( abs(fx1.norm() - fx2.norm()) < task.stopcond.tolf ) {
            flag |= OPTFLAG_TOLF;
            MOOLOG << "TOLF" << std::endl;
            break;
        }

        if ( fx2.norm() < fx1.norm() ) {
            x1 = x2;
            fx1 = fx2;
            m_bestx = x1;
        }
        else{
//           MOOLOG << "Line search "<<std::endl;
           bool found = false;
           size_t nLinsearchIter = 0;
           const size_t maxLinsearchIter = 100;
           double bestnorm = fx1.norm();
           double alpha_min = 0.0, alpha_max = 1.0;
           double alpha = 0.5*(alpha_max + alpha_min);
           while ( nLinsearchIter < maxLinsearchIter && !found ){
                v_type x21 = x1 + alpha*dx;
                v_type fx21 = (*task.target)(x21);
                ++m_fevals;
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
//                MOOLOG << " not found" << std::endl;
                break;
            }
            else{
//                MOOLOG << "alpha = " << alpha << std::endl;
            }
            x1 = x2;
            fx1 = fx2;
            m_bestx = x1;
        }
//        MOOLOG << "bestx " << std::endl << _bestx.transpose() << std::endl;
        MOOLOG  << "SolverNReg::solve: iter = " << iter <<  " err.norm = " << fx1.norm() << std::endl;
        ++iter;

    }
MOOLOG << "SolverNReg::solve - finished with flag = " << flag << " fevals " << m_fevals << std::endl;
    return 0;
}


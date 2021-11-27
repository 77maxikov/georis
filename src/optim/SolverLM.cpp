#include "SolverLM.h"
#include <stdexcept>
#include "scaler.h"

//#include <Eigen/Eigenvalues>
#include "mooLog.h"

SolverLM::SolverLM() {
    _fevals = 0;
    _mu = 1;
    _mu_inc = 2;
    _mu_dec = 2;

}
void SolverLM::setParam(const std::valarray<double>&parm) {
    if (parm.size() < 3)
        throw std::invalid_argument("SolverLM::setParam - not enough params!");
    _mu = parm[0];
    _mu_inc = parm[1];
    _mu_dec = parm[2];
}
void SolverLM::solve(const SolveTask &task) {
    if (!(*task.target).hasJacob()) throw std::invalid_argument("SolverLM::target function should provide Jacobian");
    v_type x1(task.x0);
    _bestx = x1;
    v_type fx1 = (*task.target)(x1);++_fevals;
    v_type scaler = fx1.cwiseAbs();
    for (Eigen::Index k = 0;k < scaler.size();++k)
        if ( scaler(k) < 1e-0 )
            scaler(k) = 1;
        else
            scaler(k) = 1/scaler(k);

    IOptFuncN* target = task.target;

    ScaledOptFuncN scaledtarget(task.target, scaler);
    if (0){
        target = &scaledtarget;
        fx1 = fx1.array().colwise()*scaler.array();
    }
    MOOLOG << "SolverLM::solve - initial values: " << std::endl << fx1 << std::endl;
    unsigned flag = 0;
    v_type dx(x1.size());
    size_t iter = 0;
    while (1) {
//std::cout << "SolverLM::solve - x = "<< std::endl << x1 << std::endl;
        MOOLOG << "SolverLM::solve - fx1  = " << std::endl << fx1 <<", norm = "<< fx1.norm() << std::endl;

        m_type jacob = target->getJacob(x1);
        //MOOLOG << "SolverLM::solve - J:" << std::endl << jacob << std::endl;

        /*std::cout << "=============" << std::endl;
        std::cout << "Jacob:" << std::endl << jacob << std::endl;
        std::cout << "=============" << std::endl;
        std::cout << "mu:" << std::endl << _mu << std::endl;
        std::cout << "=============" << std::endl;
*/
        m_type nf = -jacob.transpose()*fx1;
        m_type HH = jacob.transpose()*jacob;
        //MOOLOG << "SolverLM::solve - HH: " << std::endl << HH << std::endl;
        double detHH = HH.determinant();
        MOOLOG << "SolverLM::solve - det(HH): " << HH.determinant() << std::endl;

        do {
            m_type H = HH;
            if ( std::abs(detHH) < 1e-8 )
                for (int k = 0; k<x1.size(); ++k)
                    H(k,k) += _mu;
            else
                for (int k = 0; k<x1.size(); ++k)
                    H(k,k) *= ( 1 + _mu);
//            std::cout << "Damped H = " << std::endl <<H << std::endl;
//	        Eigen::EigenSolver<m_type> sol(H);
//std::cout << "The eigenvalues of H are:" << std::endl << sol.eigenvalues() << std::endl;
            MOOLOG << "SolverLM::solve - iter: " << iter <<" det(H):" << H.determinant() << std::endl;

            dx = H.fullPivLu().solve(nf);

            double rel_error = (H*dx - nf).norm() / nf.norm();
            MOOLOG << "SolverLM::solve - iter: " << iter <<" rel_error: " << rel_error << " dx.norm: " << dx.norm() << std::endl;

            if ( dx.norm() < task.stopcond.tolx ){
                    MOOLOG << "SolverLM::solve - dx.norm() < epsi" <<std::endl;
					break;
            }

            v_type x2(x1 + dx);
            v_type fx2 = (*target)(x2);
            ++_fevals;
            if ( (fx1-fx2).norm() < task.stopcond.tolf ) {
                    MOOLOG << "SolverLM::solve - df.norm() < epsi" <<std::endl;
					flag |= OPTFLAG_TOLF;
					break;
            }

            if ( fx2.norm() < fx1.norm() ) {
                _mu = std::max(_mu/_mu_dec,1e-8);
                x1 = x2;
                fx1 = fx2;
                _bestx = x1;
                break;
            } else
                _mu = std::min(_mu*_mu_inc,1e8);
MOOLOG << "_mu = " << _mu << std::endl;
        } while (1e-8< _mu && _mu < 1e8);

        if (_fevals > task.stopcond.fevals) {
            flag |= OPTFLAG_FEVALS;
            break;
        }
        if ( fx1.norm() < task.stopcond.tolf) {
                //MOOLOG << "SolverLM::solve - AHTUNG !!!  " << std::endl;
            flag |= OPTFLAG_TOLF;
            break;
        }
        if ( dx.norm() < task.stopcond.tolx ) {
            flag |= OPTFLAG_TOLX;
            break;
        }
        if ( flag & OPTFLAG_TOLF )
				break;

        ++iter;
    }
    MOOLOG << "SolverLM::solve - finished with flag = " << flag << " fevals " << _fevals << std::endl;
    //(*task.target)(_bestx);
}


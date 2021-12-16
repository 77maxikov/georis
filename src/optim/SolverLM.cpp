#include "SolverLM.h"
//#include <Eigen/Eigenvalues>
#include <stdexcept>
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

    MOOLOG << "SolverLM::solve - initial values: " << std::endl << fx1 << std::endl;
    unsigned flag = 0;
    v_type dx(x1.size());
    size_t iter = 0;
    while (1) {
        MOOLOG << "SolverLM::solve - iter:" << std::setw(6) << iter << " f norm:" << std::setw(15) << fx1.norm();// fx1  = " << std::endl << fx1 <<",

        m_type jacob = task.target->getJacob(x1);

        m_type nf = -jacob.transpose()*fx1;
        m_type HH = jacob.transpose()*jacob;
        //m_type HH = jacob*jacob.transpose();
        //MOOLOG << "SolverLM::solve - HH: " << std::endl << HH << std::endl;
        double detHH = HH.determinant();
        MOOLOG << " det(HH):" <<std::setw(15) <<  HH.determinant();
        if ( abs(HH.determinant()) < 1e-8 )
            MOOLOG << std::endl << jacob << std::endl;
        double nu = 2;
        do {
            m_type H = HH;
            if ( std::abs(detHH) < 1e-8 ){
                for (int k = 0; k < H.rows(); ++k)
                    H(k,k) += _mu;
                //MOOLOG << std::endl << H << std::endl;
            }
            else
                for (int k = 0; k < H.rows(); ++k)
                    H(k,k) *= ( 1 + _mu);
//          std::cout << "Damped H = " << std::endl <<H << std::endl;
//	        Eigen::EigenSolver<m_type> sol(H);
//std::cout << "The eigenvalues of H are:" << std::endl << sol.eigenvalues() << std::endl;
            MOOLOG << " det(H):" << std::setw(15) << H.determinant();

            dx = H.fullPivLu().solve(nf);

            //dx  = jacob.transpose() * sol;
            double rel_error = (H*dx - nf).norm() / nf.norm();
            MOOLOG << " rel_error:" << std::setw(15) << rel_error << " dx.norm:"<< std::setw(15) << dx.norm() << std::endl;

            if ( dx.norm() < task.stopcond.tolx ){
                    MOOLOG << "SolverLM::solve - dx.norm() < epsi" <<std::endl;
					break;
            }

            v_type x2(x1 + dx);
            v_type fx2 = (*task.target)(x2);
            ++_fevals;
            double dfnorm = fx1.norm() - fx2.norm();

            if ( abs(dfnorm) < task.stopcond.tolf ) {
                    MOOLOG << "SolverLM::solve - df.norm() < epsi" <<std::endl;
					flag |= OPTFLAG_TOLF;
					break;
            }
            // Calc gain ratio
            double gainRatio = dfnorm/(0.5*dx.transpose()*(_mu*dx + nf));

            if ( gainRatio > 0 ) {
                // x2 is better than x1
                //_mu = std::max(_mu/_mu_dec,1e-8);
                _mu *= std::max( 1.0/3, 1  - (2*gainRatio-1)*(2*gainRatio-1)*(2*gainRatio-1) );
                nu = 2;
                x1 = x2;
                fx1 = fx2;
                _bestx = x1;
                break;
            } else{
                //_mu = std::min(_mu*_mu_inc,1e8);
                _mu *= nu; nu *= 2;
            }
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


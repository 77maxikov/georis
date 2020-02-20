#include "SolverCeres.h"
#include "ceres/ceres.h"
#include <vector>
#include <iostream>
using namespace ceres;

SolverCeres::SolverCeres() {
    //ctor
}
void SolverCeres::setParam(const std::valarray<double>&parm) {

}

class GeoCostFunction:public ceres::CostFunction {
    const SolveTask *_task;
public:
    GeoCostFunction(const SolveTask *task) {
        _task = task;
        mutable_parameter_block_sizes()->push_back(_task->x0.size());
        set_num_residuals(_task->target->outDim());
    }
    virtual bool Evaluate(double const* const* parameters,
                          double* residuals,
                          double** jacobians)const {
        v_type x(_task->x0.size());
        for (int k = 0; k < x.size(); ++k)
            x(k) = parameters[0][k];
		std::cout <<"ACHTUNG !!!" <<std::endl;

        v_type res = (*(_task->target))(x);
        for (int k = 0; k < res.size(); ++k) {
            residuals[k] = res(k);
        }
        if (jacobians ) {
            m_type jac = _task->target->getJacob(x);
            int k = 0;
            for (int r = 0; r < res.size(); ++r)
                for (int c = 0; c < x.size(); ++c ) {
                    jacobians[0][k++] = jac(r,c);
                }
        }

        return true;
    };
};

void SolverCeres::solve(const SolveTask &task) {
    double *x = new double[task.x0.size()];
    std::vector<double*> xx;
    xx.push_back(x);
    for (int k = 0; k<task.x0.size() ; ++k)
        x[k] = task.x0(k);
    ceres::Problem problem;
    // Set up the only cost function (also known as residual). This uses
    // auto-differentiation to obtain the derivative (jacobian).

    GeoCostFunction* cost_function =
        new GeoCostFunction(&task);

    problem.AddResidualBlock(cost_function, NULL, xx);

    // Run the solver!
    Solver::Options options;
    options.minimizer_type = LINE_SEARCH;
    options.linear_solver_type = ceres::DENSE_QR;
	//options.minimizer_type = TRUST_REGION;
    options.minimizer_progress_to_stdout = true;
    //options.minimizer_progress_to_stdout = false;
    //options.logging_type = SILENT;
    //options.logging_type = PER_MINIMIZER_ITERATION;
    //options.gradient_check_relative_precision = 0;

    Solver::Summary summary;
    Solve(options, &problem, &summary);
    std::cout << "Q7" << std::endl;
    //std::cout << summary.BriefReport() << "\n";
    //std::cout << "x : " << initial_x
    //<< " -> " << x << "\n";

    delete[] x;
}

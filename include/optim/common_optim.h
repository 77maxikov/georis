#ifndef __COMMONH
#define __COMMONH

#include <Eigen/Dense>
#include <Eigen/Sparse>

typedef double s_type;
typedef Eigen::VectorXd v_type;
typedef Eigen::MatrixXd m_type;
typedef Eigen::SparseMatrix<double> spam_type;

s_type norm2(const v_type&v);
#endif

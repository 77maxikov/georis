#ifndef __COMMONH
#define __COMMONH

#include <Eigen/Dense>
typedef double s_type;
typedef Eigen::VectorXd v_type;
typedef Eigen::MatrixXd m_type;

s_type norm2(const v_type&v);
#endif

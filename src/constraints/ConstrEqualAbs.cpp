#include "ConstrEqualAbs.h"
#include <cmath>

double georis::ConstrEqualAbs::error()const{
    return std::abs(*_p1 -*_p2);
}
double georis::ConstrEqualAbs::grad(const double *var)const{
    double w = (*_p1 - *_p2)<0?-1:1;
    if (var == _p1)
        return w;
    if (var == _p2)
        return -w;
    return 0;
}

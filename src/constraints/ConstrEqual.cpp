#include "ConstrEqual.h"
#include <cmath>

double georis::ConstrEqual::error()const{
    return (*_p1 -*_p2);
}
double georis::ConstrEqual::grad(const double *var)const{
    if (var == _p1) return 1;
    if (var == _p2) return -1;
    return 0;
}

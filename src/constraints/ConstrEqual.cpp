#include "ConstrEqual.h"
#include <cmath>

double georis::ConstrEqual::error()const{
    return (*_p1->pval -*_p2->pval);
}
double georis::ConstrEqual::grad(const paramProxy *var)const{
    if (var == _p1) return 1;
    if (var == _p2) return -1;
    return 0;
}

double georis::ConstrMidVal::error()const{
    return (2**_mid->pval - *_p1->pval -*_p2->pval);
}
double georis::ConstrMidVal::grad(const paramProxy *var)const{
    if (var == _mid ) return 2;
    if (var == _p1) return -1;
    if (var == _p2) return -1;
    return 0;
}

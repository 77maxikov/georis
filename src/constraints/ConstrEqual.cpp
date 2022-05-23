#include "ConstrEqual.h"
#include <cmath>

double georis::ConstrEqual::error()const{
    return (*_p1->pval -*_p2->pval);
}
georis::DiDelegate georis::ConstrEqual::grad(const paramProxy *var){
    if (var == _p1)
        return georis::DiDelegate::from_method<georis::ConstrEqual,&georis::ConstrEqual::dp1>(this);
    if (var == _p2)
        return georis::DiDelegate::from_method<georis::ConstrEqual,&georis::ConstrEqual::dp2>(this);
    return DiDelegate();
}

double georis::ConstrMidVal::error()const{
    return (2**_mid->pval - *_p1->pval -*_p2->pval);
}
georis::DiDelegate georis::ConstrMidVal::grad(const paramProxy *var){
    if (var == _mid )
        return georis::DiDelegate::from_method<georis::ConstrMidVal,&georis::ConstrMidVal::dmid>(this);
    if (var == _p1 || var == _p2 ) return georis::DiDelegate::from_method<georis::ConstrMidVal,&georis::ConstrMidVal::dp>(this);
    return DiDelegate();
}

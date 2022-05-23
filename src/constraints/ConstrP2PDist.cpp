#include "ConstrP2PDist.h"
#include <cmath>
#include <stdexcept>

georis::ConstrP2PDist::ConstrP2PDist(const georis::point2r &p1,const georis::point2r &p2,paramProxy *dist){
    _p1x = p1.x;
    _p1y = p1.y;
    _p2x = p2.x;
    _p2y = p2.y;
    _dist = dist;
}
double georis::ConstrP2PDist::d2()const{
    return (*_p1x->pval -*_p2x->pval)*(*_p1x->pval -*_p2x->pval) + (*_p1y->pval -*_p2y->pval)*(*_p1y->pval -*_p2y->pval);
}
double georis::ConstrP2PDist::error()const{
    return std::sqrt(d2()) - *_dist->pval;
}

georis::DiDelegate georis::ConstrP2PDist::grad(const paramProxy *var){
    if (var == _p1x)
        return georis::DiDelegate::from_method<georis::ConstrP2PDist,&georis::ConstrP2PDist::dp1x>(this);
    if (var == _p1y)
        return georis::DiDelegate::from_method<georis::ConstrP2PDist,&georis::ConstrP2PDist::dp1y>(this);
    if (var == _p2x)
        return georis::DiDelegate::from_method<georis::ConstrP2PDist,&georis::ConstrP2PDist::dp2x>(this);
    if (var == _p2y)
        return georis::DiDelegate::from_method<georis::ConstrP2PDist,&georis::ConstrP2PDist::dp2y>(this);

    return georis::DiDelegate();
}

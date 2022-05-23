#include "ConstrP2LDist.h"
#include <cmath>
#include <stdexcept>


georis::ConstrPOnL::ConstrPOnL(const georis::point2r &p, const georis::line2r &l) {
    _px = p.x;
    _py = p.y;
    _l1x = l.beg.x;
    _l1y = l.beg.y;
    _l2x = l.end.x;
    _l2y = l.end.y;
}
double georis::ConstrPOnL::error()const {
    prepare();
    return (lpx*ay-lpy*ax)/d;
}
georis::DiDelegate georis::ConstrPOnL::grad(const paramProxy *var) {

    if (var == _px)
        return georis::DiDelegate::from_method<georis::ConstrPOnL,&georis::ConstrPOnL::dpx>(this);
    if (var == _py)
        return georis::DiDelegate::from_method<georis::ConstrPOnL,&georis::ConstrPOnL::dpy>(this);
    if (var == _l1x)
        return georis::DiDelegate::from_method<georis::ConstrPOnL,&georis::ConstrPOnL::dl1x>(this);
    if (var == _l1y)
        return georis::DiDelegate::from_method<georis::ConstrPOnL,&georis::ConstrPOnL::dl1y>(this);
    if (var == _l2x)
        return georis::DiDelegate::from_method<georis::ConstrPOnL,&georis::ConstrPOnL::dl2x>(this);
    if (var == _l2y)
        return georis::DiDelegate::from_method<georis::ConstrPOnL,&georis::ConstrPOnL::dl2y>(this);

    return DiDelegate();
}




georis::ConstrP2LDist::ConstrP2LDist(const georis::point2r &p, const georis::line2r &l,paramProxy *dist,bool varDist) {
    _px = p.x;
    _py = p.y;
    _l1x = l.beg.x;
    _l1y = l.beg.y;
    _l2x = l.end.x;
    _l2y = l.end.y;
    _dist = dist;
    _varDist = varDist;
}
double georis::ConstrP2LDist::error()const {
    prepare();
    return std::abs(lpx*ay-lpy*ax)/d - *_dist->pval;
}
georis::DiDelegate georis::ConstrP2LDist::grad(const paramProxy *var){
    prepare();

    if (var == _px)
        return georis::DiDelegate::from_method<georis::ConstrP2LDist,&georis::ConstrP2LDist::dpx>(this);
    if (var == _py)
        return georis::DiDelegate::from_method<georis::ConstrP2LDist,&georis::ConstrP2LDist::dpy>(this);
    if (var == _l1x)
        return georis::DiDelegate::from_method<georis::ConstrP2LDist,&georis::ConstrP2LDist::dl1x>(this);
    if (var == _l1y)
        return georis::DiDelegate::from_method<georis::ConstrP2LDist,&georis::ConstrP2LDist::dl1y>(this);
    if (var == _l2x)
        return georis::DiDelegate::from_method<georis::ConstrP2LDist,&georis::ConstrP2LDist::dl2x>(this);
    if (var == _l2y)
        return georis::DiDelegate::from_method<georis::ConstrP2LDist,&georis::ConstrP2LDist::dl2y>(this);
    if (var == _dist && _varDist ){
        return georis::DiDelegate::from_method<georis::ConstrP2LDist,&georis::ConstrP2LDist::ddist>(this);
    }

    return georis::DiDelegate();
}

georis::ConstrP2PLDist::ConstrP2PLDist(const georis::point2r &p1,const georis::point2r &p2, const georis::line2r &l) {
    _p1x = p1.x;
    _p1y = p1.y;
    _p2x = p2.x;
    _p2y = p2.y;
    _l1x = l.beg.x;
    _l1y = l.beg.y;
    _l2x = l.end.x;
    _l2y = l.end.y;
}

double georis::ConstrP2PLDist::error()const {
    double ax = *_l2x->pval - *_l1x->pval;
    double ay = *_l2y->pval - *_l1y->pval;
    double d = std::sqrt(ax*ax+ay*ay);
    if ( d < epsi ) throw std::runtime_error("ConstrP2PLDist::not a line!");

    double p1lx = *_p1x->pval - *_l1x->pval;
    double p1ly = *_p1y->pval - *_l1y->pval;
    double p1p2x = *_p2x->pval - *_p1x->pval;
    double p1p2y = *_p2y->pval - *_p1y->pval;


    return (p1p2x*p1p2x + p1p2y*p1p2y)-(p1lx*ay-p1ly*ax)*(p1lx*ay-p1ly*ax)/d/d;
}
georis::DiDelegate georis::ConstrP2PLDist::grad(const paramProxy *var) {
    if (var == _p1x)
        return georis::DiDelegate::from_method<georis::ConstrP2PLDist,&georis::ConstrP2PLDist::dp1x>(this);
    if (var == _p1y)
        return georis::DiDelegate::from_method<georis::ConstrP2PLDist,&georis::ConstrP2PLDist::dp1y>(this);
    if (var == _p2x)
        return georis::DiDelegate::from_method<georis::ConstrP2PLDist,&georis::ConstrP2PLDist::dp2x>(this);
    if (var == _p2y)
        return georis::DiDelegate::from_method<georis::ConstrP2PLDist,&georis::ConstrP2PLDist::dp2y>(this);
    if (var == _l1x)
        return georis::DiDelegate::from_method<georis::ConstrP2PLDist,&georis::ConstrP2PLDist::dl1x>(this);
    if (var == _l1y)
        return georis::DiDelegate::from_method<georis::ConstrP2PLDist,&georis::ConstrP2PLDist::dl1y>(this);
    if (var == _l2x)
        return georis::DiDelegate::from_method<georis::ConstrP2PLDist,&georis::ConstrP2PLDist::dl2x>(this);
    if (var == _l2y)
        return georis::DiDelegate::from_method<georis::ConstrP2PLDist,&georis::ConstrP2PLDist::dl2y>(this);

    return georis::DiDelegate();
}

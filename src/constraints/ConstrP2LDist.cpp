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
    double ax = *_l2x->pval - *_l1x->pval;
    double ay = *_l2y->pval - *_l1y->pval;
    double lpx = *_px->pval - *_l1x->pval;
    double lpy = *_py->pval - *_l1y->pval;
    double d = std::sqrt(ax*ax+ay*ay);
    if ( d < epsi ) throw std::runtime_error("ConstrPOnL::not a line!");
    return (lpx*ay-lpy*ax)/d;
}
double georis::ConstrPOnL::grad(const paramProxy *var)const {
    double ax = *_l2x->pval - *_l1x->pval;
    double ay = *_l2y->pval - *_l1y->pval;
    double lpx = *_px->pval - *_l1x->pval;
    double lpy = *_py->pval - *_l1y->pval;
    double d = std::sqrt(ax*ax+ay*ay);
    if ( d < epsi ) throw std::runtime_error("ConstrPOnL::not a line!");
    double S = lpx*ay-lpy*ax;

    if (var == _px){
        return ay/d;
    }
    if (var == _py){
        return (-ax)/d;
    }
    if (var == _l1x){
        return (( -*_l2y->pval + *_py->pval)*d - S*( -*_l2x->pval + *_l1x->pval)/d)/d/d;
    }
    if (var == _l1y){
        return (( -*_px->pval + *_l2x->pval)*d + S*ay/d)/d/d;
    }
    if (var == _l2x){
        return (-lpy*d - S*ax/d)/d/d;
    }
    if (var == _l2y){
        return (lpx*d - S*ay/d)/d/d;
    }
    return 0;
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
    double ax = *_l2x->pval - *_l1x->pval;
    double ay = *_l2y->pval - *_l1y->pval;
    double lpx = *_px->pval - *_l1x->pval;
    double lpy = *_py->pval - *_l1y->pval;
    double d = std::sqrt(ax*ax+ay*ay);
    if ( d < epsi ) throw std::runtime_error("ConstrP2LDist::not a line!");
    return std::abs(lpx*ay-lpy*ax)/d - *_dist->pval;
}
double georis::ConstrP2LDist::grad(const paramProxy *var)const {
    double ax = *_l2x->pval - *_l1x->pval;
    double ay = *_l2y->pval - *_l1y->pval;
    double lpx = *_px->pval - *_l1x->pval;
    double lpy = *_py->pval - *_l1y->pval;
    double d = std::sqrt(ax*ax+ay*ay);
    if ( d < epsi ) throw std::runtime_error("ConstrP2LDist::not a line!");
    double S = lpx*ay-lpy*ax;

    double w = S<0?-1:1;

    if (var == _px){
        return w*ay/d;
    }
    if (var == _py){
        return w*(-ax)/d;
    }
    if (var == _l1x){
        return w*(( -*_l2y->pval + *_py->pval)*d + S*ax/d)/d/d;
    }
    if (var == _l1y){
        return w*(( -*_px->pval + *_l2x->pval)*d - S*(-ay)/d)/d/d;
    }
    if (var == _l2x){
        return w*(( -lpy)*d - S*ax/d)/d/d;
    }
    if (var == _l2y){
        return w*(lpx*d - S*ay/d)/d/d;
    }
    if (var == _dist && _varDist ){
        return -1;
    }
    return 0;
}
/*
georis::ConstrP2LaPEq::ConstrP2LaPEq(const georis::point2r &p1, const georis::line2r &l,const georis::point2r &p2) {
    p1x = p1.x;
    p1y = p1.y;
    p2x = p2.x;
    p2y = p2.y;
    l1x = l.beg.x;
    l1y = l.beg.y;
    l2x = l.end.x;
    l2y = l.end.y;
}

double georis::ConstrP2LaPEq::error()const {
    double ax = *_l2x - *_l1x;
    double ay = *_l2y - *_l1y;

    double lp1x = *_p1x - *_l1x;
    double lp1y = *_p1y - *_l1y;

    double ppx = *_p1x - *_p2x;
    double ppy = *_p1y - *_p2y;

    double d = std::sqrt(ax*ax+ay*ay);
    if ( d < epsi ) throw std::runtime_error("ConstrP2LaPEq::not a line!");

    return std::abs( std::abs(lp1x*ay-lp1y*ax)/d - std::sqrt(ppx*ppx+ppy*ppy) );
}
double georis::ConstrP2LaPEq::grad(const double *var)const {

    double ax = *_l2x - *_l1x;
    double ay = *_l2y - *_l1y;
    double lp1x = *_p1x - *_l1x;
    double lp1y = *_p1y - *_l1y;

    double d = std::sqrt(ax*ax+ay*ay);
    if ( d < epsi ) throw std::runtime_error("ConstrP2LaPEq::not a line!");

    double S = lp1x*ay-lp1y*ax;

    double w = S<0?-1:1;

    if (var == p1x){
        return w*ay/d;
    }
    if (var == p1y){
        return w*(-ax)/d;
    }
    if (var == p2x){
        return w*ay/d;
    }
    if (var == p2y){
        return w*(-ax)/d;
    }

    if (var == l1x){
        return w*(( -*_l2y + *_p1y)*d - S*( -*_l2x + *_l1x)/d)/d/d;
    }
    if (var == l1y){
        return w*(( -*_p1x + *_l2x)*d - S*( -*_l2y + *_l1y)/d)/d/d;
    }
    if (var == l2x){
        return w*(( -*_p1y + *_l1y)*d - S*( *_l2x - *_l1x)/d)/d/d;
    }
    if (var == l2y){
        return w*(( *_p1x - *_l1x)*d - S*( *_l2y - *_l1y)/d)/d/d;
    }
}


georis::ConstrPP2LDist::ConstrPP2LDist(const georis::point2r &p1, const georis::line2r &l,const georis::point2r &p2) {
    px1 = p1.x;
    py1 = p1.y;
    px2 = p2.x;
    py2 = p2.y;
    l1x = l.beg.x;
    l1y = l.beg.y;
    l2x = l.end.x;
    l2y = l.end.y;
}

double georis::ConstrPP2LDist::error()const {
    double ax = *_l2x - *_l1x;
    double ay = *_l2y - *_l1y;

    double lpx1 = *px1 - *_l1x;
    double lpy1 = *py1 - *_l1y;
    double lpx2 = *px2 - *_l1x;
    double lpy2 = *py2 - *_l1y;

    return std::abs(std::abs(lpx1*ay-lpy1*ax)-std::abs(lpx2*ay-lpy2*ax));
}
double georis::ConstrPP2LDist::grad(const double *var)const {
/*
    double ax = *_l2x - *_l1x;
    double ay = *_l2y - *_l1y;
    double lpx1 = *px1 - *_l1x;
    double lpy1 = *py1 - *_l1y;
    double lpx2 = *px2 - *_l1x;
    double lpy2 = *py2 - *_l1y;


    double d = std::sqrt(ax*ax+ay*ay);
    if ( d < epsi ) throw std::runtime_error("ConstrP2LDist::not a line!");
    double S = lpx*ay-lpy*ax;

    double w = S<0?-1:1;

    if (var == px){
        return w*ay/d;
    }
    if (var == py){
        return w*(-ax)/d;
    }
    if (var == l1x){
        return w*(( -*_l2y + *py)*d - S*( -*_l2x + *_l1x)/d)/d/d;
    }
    if (var == l1y){
        return w*(( -*px + *_l2x)*d - S*( -*_l2y + *_l1y)/d)/d/d;
    }
    if (var == l2x){
        return w*(( -*py + *_l1y)*d - S*( *_l2x - *_l1x)/d)/d/d;
    }
    if (var == l2y){
        return w*(( *px - *_l1x)*d - S*( *_l2y - *_l1y)/d)/d/d;
    }
    if (var == _dist && _varDist ){
        return -1;
    }

    return 0;
}

*/


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
double georis::ConstrP2PLDist::grad(const paramProxy *var)const {

    double ax = *_l2x->pval - *_l1x->pval;
    double ay = *_l2y->pval - *_l1y->pval;
    /*
    double lpx1 = *_p1x - *_l1x;
    double lpy1 = *_p1y - *_l1y;
    double lpx2 = *_p2x - *_l1x;
    double lpy2 = *_p2y - *_l1y;
*/

    double d = std::sqrt(ax*ax+ay*ay);
    if ( d < epsi ) throw std::runtime_error("ConstrP2LDist::not a line!");

    if (var == _p1x)
        return 1/(sqrt(*_p2x->pval**_p2x->pval-2.0**_p2x->pval**_p1x->pval+*_p1x->pval**_p1x->pval+*_p2y->pval**_p2y->pval-2.0**_p2y->pval**_p1y->pval+*_p1y->pval**_p1y->pval))*(-2.0**_p2x->pval+2.0**_p1x->pval)/2.0-2.0*((*_p1x->pval-*_l1x->pval)*(*_l2y->pval-*_l1y->pval)-(*_p1y->pval-*_l1y->pval)*(*_l2x->pval-*_l1x->pval))/(*_l2x->pval**_l2x->pval-2.0**_l2x->pval**_l1x->pval+*_l1x->pval**_l1x->pval+*_l2y->pval**_l2y->pval-2.0**_l2y->pval**_l1y->pval+*_l1y->pval**_l1y->pval)*(*_l2y->pval-*_l1y->pval);
    if (var == _p1y)
        return 1/(sqrt(*_p2x->pval**_p2x->pval-2.0**_p2x->pval**_p1x->pval+*_p1x->pval**_p1x->pval+*_p2y->pval**_p2y->pval-2.0**_p2y->pval**_p1y->pval+*_p1y->pval**_p1y->pval))*(-2.0**_p2y->pval+2.0**_p1y->pval)/2.0-2.0*((*_p1x->pval-*_l1x->pval)*(*_l2y->pval-*_l1y->pval)-(*_p1y->pval-*_l1y->pval)*(*_l2x->pval-*_l1x->pval))/(*_l2x->pval**_l2x->pval-2.0**_l2x->pval**_l1x->pval+*_l1x->pval**_l1x->pval+*_l2y->pval**_l2y->pval-2.0**_l2y->pval**_l1y->pval+*_l1y->pval**_l1y->pval)*(-*_l2x->pval+*_l1x->pval);
    if (var == _p2x)
        return (*_p2x->pval-*_p1x->pval)/sqrt(*_p2x->pval**_p2x->pval-2.0**_p2x->pval**_p1x->pval+*_p1x->pval**_p1x->pval+*_p2y->pval**_p2y->pval-2.0**_p2y->pval**_p1y->pval+*_p1y->pval**_p1y->pval);
    if (var == _p2y)
        return (*_p2y->pval-*_p1y->pval)/sqrt(*_p2x->pval**_p2x->pval-2.0**_p2x->pval**_p1x->pval+*_p1x->pval**_p1x->pval+*_p2y->pval**_p2y->pval-2.0**_p2y->pval**_p1y->pval+*_p1y->pval**_p1y->pval);
    if (var == _l1x)
        return 2.0*(*_p1x->pval**_l2y->pval-*_p1x->pval**_l1y->pval-*_l1x->pval**_l2y->pval-*_p1y->pval**_l2x->pval+*_p1y->pval**_l1x->pval+*_l1y->pval**_l2x->pval)*(*_l2y->pval-*_l1y->pval)*(*_l2y->pval**_l2y->pval-*_p1y->pval**_l2y->pval-*_l2y->pval**_l1y->pval+*_p1y->pval**_l1y->pval+*_p1x->pval**_l1x->pval+*_l2x->pval**_l2x->pval-*_l2x->pval**_l1x->pval-*_p1x->pval**_l2x->pval)/pow(*_l2x->pval**_l2x->pval-2.0**_l2x->pval**_l1x->pval+*_l1x->pval**_l1x->pval+*_l2y->pval**_l2y->pval-2.0**_l2y->pval**_l1y->pval+*_l1y->pval**_l1y->pval,2.0);
    if (var == _l1y)
        return -2.0*(*_p1x->pval**_l2y->pval-*_p1x->pval**_l1y->pval-*_l1x->pval**_l2y->pval-*_p1y->pval**_l2x->pval+*_p1y->pval**_l1x->pval+*_l1y->pval**_l2x->pval)*(*_l2x->pval-*_l1x->pval)*(*_l2y->pval**_l2y->pval-*_p1y->pval**_l2y->pval-*_l2y->pval**_l1y->pval+*_p1y->pval**_l1y->pval+*_p1x->pval**_l1x->pval+*_l2x->pval**_l2x->pval-*_l2x->pval**_l1x->pval-*_p1x->pval**_l2x->pval)/pow(*_l2x->pval**_l2x->pval-2.0**_l2x->pval**_l1x->pval+*_l1x->pval**_l1x->pval+*_l2y->pval**_l2y->pval-2.0**_l2y->pval**_l1y->pval+*_l1y->pval**_l1y->pval,2.0);
    if (var == _l2x)
        return -2.0*(*_p1x->pval**_l2y->pval-*_p1x->pval**_l1y->pval-*_l1x->pval**_l2y->pval-*_p1y->pval**_l2x->pval+*_p1y->pval**_l1x->pval+*_l1y->pval**_l2x->pval)*(*_l2y->pval-*_l1y->pval)*(*_l2y->pval**_l1y->pval-*_p1y->pval**_l2y->pval+*_p1x->pval**_l1x->pval+*_p1y->pval**_l1y->pval-*_p1x->pval**_l2x->pval-*_l1x->pval**_l1x->pval-*_l1y->pval**_l1y->pval+*_l2x->pval**_l1x->pval)/pow(*_l2x->pval**_l2x->pval-2.0**_l2x->pval**_l1x->pval+*_l1x->pval**_l1x->pval+*_l2y->pval**_l2y->pval-2.0**_l2y->pval**_l1y->pval+*_l1y->pval**_l1y->pval,2.0);
    if (var == _l2y)
        return 2.0*(*_p1x->pval**_l2y->pval-*_p1x->pval**_l1y->pval-*_l1x->pval**_l2y->pval-*_p1y->pval**_l2x->pval+*_p1y->pval**_l1x->pval+*_l1y->pval**_l2x->pval)*(*_l2x->pval-*_l1x->pval)*(*_l2y->pval**_l1y->pval-*_p1y->pval**_l2y->pval+*_p1x->pval**_l1x->pval+*_p1y->pval**_l1y->pval-*_p1x->pval**_l2x->pval-*_l1x->pval**_l1x->pval-*_l1y->pval**_l1y->pval+*_l2x->pval**_l1x->pval)/pow(*_l2x->pval**_l2x->pval-2.0**_l2x->pval**_l1x->pval+*_l1x->pval**_l1x->pval+*_l2y->pval**_l2y->pval-2.0**_l2y->pval**_l1y->pval+*_l1y->pval**_l1y->pval,2.0);

    return 0;
}

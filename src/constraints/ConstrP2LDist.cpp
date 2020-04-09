#include "ConstrP2LDist.h"
#include <cmath>
#include <stdexcept>


georis::ConstrP2LDist::ConstrP2LDist(const georis::point2r &p, const georis::line2r &l,double *dist,bool varDist) {
    px = p.x;
    py = p.y;
    l1x = l.beg.x;
    l1y = l.beg.y;
    l2x = l.end.x;
    l2y = l.end.y;
    _dist = dist;
    _varDist = varDist;
}
double georis::ConstrP2LDist::error()const {
    double ax = *l2x - *l1x;
    double ay = *l2y - *l1y;
    double lpx = *px - *l1x;
    double lpy = *py - *l1y;
    double d = std::sqrt(ax*ax+ay*ay);
    if ( d < epsi ) throw std::runtime_error("ConstrP2LDist::not a line!");
    return std::abs(lpx*ay-lpy*ax)/d-*_dist;
}
double georis::ConstrP2LDist::grad(const double *var)const {
    double ax = *l2x - *l1x;
    double ay = *l2y - *l1y;
    double lpx = *px - *l1x;
    double lpy = *py - *l1y;
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
        return w*(( -*l2y + *py)*d - S*( -*l2x + *l1x)/d)/d/d;
    }
    if (var == l1y){
        return w*(( -*px + *l2x)*d - S*( -*l2y + *l1y)/d)/d/d;
    }
    if (var == l2x){
        return w*(( -*py + *l1y)*d - S*( *l2x - *l1x)/d)/d/d;
    }
    if (var == l2y){
        return w*(( *px - *l1x)*d - S*( *l2y - *l1y)/d)/d/d;
    }
    if (var == _dist && _varDist ){
        return -1;
    }
    return 0;
}

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
    double ax = *l2x - *l1x;
    double ay = *l2y - *l1y;

    double lp1x = *p1x - *l1x;
    double lp1y = *p1y - *l1y;

    double ppx = *p1x - *p2x;
    double ppy = *p1y - *p2y;

    double d = std::sqrt(ax*ax+ay*ay);
    if ( d < epsi ) throw std::runtime_error("ConstrP2LaPEq::not a line!");

    return std::abs( std::abs(lp1x*ay-lp1y*ax)/d - std::sqrt(ppx*ppx+ppy*ppy) );
}
double georis::ConstrP2LaPEq::grad(const double *var)const {

    double ax = *l2x - *l1x;
    double ay = *l2y - *l1y;
    double lp1x = *p1x - *l1x;
    double lp1y = *p1y - *l1y;

    double lp2x = *p2x - *l1x;
    double lp2y = *p2y - *l1y;

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
        return w*(( -*l2y + *p1y)*d - S*( -*l2x + *l1x)/d)/d/d;
    }
    if (var == l1y){
        return w*(( -*p1x + *l2x)*d - S*( -*l2y + *l1y)/d)/d/d;
    }
    if (var == l2x){
        return w*(( -*p1y + *l1y)*d - S*( *l2x - *l1x)/d)/d/d;
    }
    if (var == l2y){
        return w*(( *p1x - *l1x)*d - S*( *l2y - *l1y)/d)/d/d;
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
    double ax = *l2x - *l1x;
    double ay = *l2y - *l1y;

    double lpx1 = *px1 - *l1x;
    double lpy1 = *py1 - *l1y;
    double lpx2 = *px2 - *l1x;
    double lpy2 = *py2 - *l1y;

    return std::abs(std::abs(lpx1*ay-lpy1*ax)-std::abs(lpx2*ay-lpy2*ax));
}
double georis::ConstrPP2LDist::grad(const double *var)const {
/*
    double ax = *l2x - *l1x;
    double ay = *l2y - *l1y;
    double lpx1 = *px1 - *l1x;
    double lpy1 = *py1 - *l1y;
    double lpx2 = *px2 - *l1x;
    double lpy2 = *py2 - *l1y;


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
        return w*(( -*l2y + *py)*d - S*( -*l2x + *l1x)/d)/d/d;
    }
    if (var == l1y){
        return w*(( -*px + *l2x)*d - S*( -*l2y + *l1y)/d)/d/d;
    }
    if (var == l2x){
        return w*(( -*py + *l1y)*d - S*( *l2x - *l1x)/d)/d/d;
    }
    if (var == l2y){
        return w*(( *px - *l1x)*d - S*( *l2y - *l1y)/d)/d/d;
    }
    if (var == _dist && _varDist ){
        return -1;
    }
    */
    return 0;
}

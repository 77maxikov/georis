#include "ConstrL2LAngle.h"
#include <cmath>
#include <stdexcept>
#include "common_math.h"

georis::ConstrL2LAngle::ConstrL2LAngle(const georis::line2r &l1, const georis::line2r &l2,paramProxy *angle):ConstrL2L(l1,l2){
    if (angle == 0) throw std::invalid_argument("ConstrL2LAngle::invalid angle");
    _angle = angle;
}
double georis::ConstrL2LAngle::error()const{
    double a1x = *l12x->pval - *l11x->pval;
    double a1y = *l12y->pval - *l11y->pval;
    double a2x = *l22x->pval - *l21x->pval;
    double a2y = *l22y->pval - *l21y->pval;

    if ( a1x*a1x + a1y*a1y < epsi*epsi ) throw std::runtime_error("ConstrL2LAngle::not a line 1!");
    if ( a2x*a2x + a2y*a2y < epsi*epsi ) throw std::runtime_error("ConstrL2LAngle::not a line 2!");
    double c = a1x*a2x + a1y*a2y;
    double s = a1x*a2y - a1y*a2x;

    return atan2(s,c) - deg2rad(*_angle->pval);
}
double georis::ConstrL2LAngle::grad(const paramProxy *var)const{
    double a1x = *l12x->pval - *l11x->pval;
    double a1y = *l12y->pval - *l11y->pval;
    double a2x = *l22x->pval - *l21x->pval;
    double a2y = *l22y->pval - *l21y->pval;

    if ( a1x*a1x + a1y*a1y < epsi*epsi ) throw std::runtime_error("ConstrL2LAngle::not a line 1!");
    if ( a2x*a2x + a2y*a2y < epsi*epsi ) throw std::runtime_error("ConstrL2LAngle::not a line 2!");
    double c = a1x*a2x + a1y*a2y;
    double s = a1x*a2y - a1y*a2x;

    if (var == l11x)
        return (-a2y*c + a2x*s)/(s*s + c*c);
    if (var == l11y)
        return (a2x*c + a2y*s)/(s*s + c*c);
    if (var == l12x)
        return (a2y*c - a2x*s)/(s*s + c*c);
    if (var == l12y)
        return (-a2x*c - a2y*s)/(s*s + c*c);
    if (var == l21x)
        return (a1y*c + a1x*s)/(s*s + c*c);
    if (var == l21y)
        return (a1x*c + a1y*s)/(s*s + c*c);
    if (var == l22x)
        return (-a1y*c - a1x*s)/(s*s + c*c);
    if (var == l22y)
        return (a1x*c - a1y*s)/(s*s + c*c);
    return 0;
}


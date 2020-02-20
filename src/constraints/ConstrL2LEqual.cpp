#include "ConstrL2LEqual.h"
#include <cmath>
#include <stdexcept>

double georis::ConstrL2LEqual::error()const {
    double a1x = *l12x - *l11x;
    double a1y = *l12y - *l11y;
    double a2x = *l22x - *l21x;
    double a2y = *l22y - *l21y;

    if ( a1x*a1x + a1y*a1y < epsi*epsi ) throw std::runtime_error("ConstrL2LEqual::not a line 1!");
    if ( a2x*a2x + a2y*a2y < epsi*epsi ) throw std::runtime_error("ConstrL2LEqual::not a line 2!");
    return a1x*a1x + a1y*a1y - a2x*a2x - a2y*a2y;
}
double georis::ConstrL2LEqual::grad(const double *var)const {
    double a1x = *l12x - *l11x;
    double a1y = *l12y - *l11y;
    double a2x = *l22x - *l21x;
    double a2y = *l22y - *l21y;

    if ( a1x*a1x + a1y*a1y < epsi*epsi ) throw std::runtime_error("ConstrL2LEqual::not a line 1!");
    if ( a2x*a2x + a2y*a2y < epsi*epsi ) throw std::runtime_error("ConstrL2LEqual::not a line 2!");

    if (var == l11x)
        return -2*a1x;
    if (var == l11y)
        return -2*a1y;
    if (var == l12x)
        return 2*a1x;
    if (var == l12y)
        return 2*a1y;
    if (var == l21x)
        return 2*a2x;
    if (var == l21y)
        return 2*a2y;
    if (var == l22x)
        return -2*a2x;
    if (var == l22y)
        return -2*a2y;
    return 0;
}


#include "ConstrL2LOrthoAbs.h"
#include <cmath>
#include <stdexcept>
#include <iostream>

double georis::ConstrL2LOrthoAbs::error()const{
    double a1x = *l12x - *l11x;
    double a1y = *l12y - *l11y;
    double a2x = *l22x - *l21x;
    double a2y = *l22y - *l21y;

	if ( a1x*a1x + a1y*a1y < epsi*epsi ) throw std::runtime_error("ConstrL2LOrthoAbs - not a line 1!");
	if ( a2x*a2x + a2y*a2y < epsi*epsi ) throw std::runtime_error("ConstrL2LOrthoAbs - not a line 2!");
	double er = a1x*a2x + a1y*a2y;
	return std::abs(er);
}
double georis::ConstrL2LOrthoAbs::grad(const double *var)const{
    double a1x = *l12x - *l11x;
    double a1y = *l12y - *l11y;
    double a2x = *l22x - *l21x;
    double a2y = *l22y - *l21y;

	if ( a1x*a1x + a1y*a1y < epsi*epsi ) throw std::runtime_error("ConstrL2LOrthoAbs::grad - not a line 1!");
	if ( a2x*a2x + a2y*a2y < epsi*epsi ) throw std::runtime_error("ConstrL2LOrthoAbs::grad - not a line 2!");

	double w = ( (a1x*a2x + a1y*a2y)<0)?-1:1;

    if (var == l11x)
        return w*(*l21x - *l22x);
    if (var == l11y)
        return w*(*l21y - *l22y);
    if (var == l12x)
        return w*(*l22x - *l21x);
    if (var == l12y)
        return w*(*l22y - *l21y);
    if (var == l21x)
        return w*(*l11x - *l12x);
    if (var == l21y)
        return w*(*l11y - *l12y);
    if (var == l22x)
        return w*(*l12x - *l11x);
    if (var == l22y)
        return w*(*l12y - *l11y);
    return 0;
}


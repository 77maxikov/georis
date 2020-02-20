#include "ConstrL2LOrtho2.h"
#include <cmath>
#include <stdexcept>
#include <iostream>

double georis::ConstrL2LOrtho2::error()const{
    double a1x = *l12x - *l11x;
    double a1y = *l12y - *l11y;
    double a2x = *l22x - *l21x;
    double a2y = *l22y - *l21y;

	if ( a1x*a1x + a1y*a1y < epsi*epsi ) throw std::runtime_error("ConstrL2LOrtho2 - not a line 1!");
	if ( a2x*a2x + a2y*a2y < epsi*epsi ) throw std::runtime_error("ConstrL2LOrtho2 - not a line 2!");
	double er = a1x*a2x + a1y*a2y;
	return er*er;
}
double georis::ConstrL2LOrtho2::grad(const double *var)const{
    double a1x = *l12x - *l11x;
    double a1y = *l12y - *l11y;
    double a2x = *l22x - *l21x;
    double a2y = *l22y - *l21y;

	if ( a1x*a1x + a1y*a1y < epsi*epsi ) throw std::runtime_error("ConstrL2LOrtho2 - not a line 1!");
	if ( a2x*a2x + a2y*a2y < epsi*epsi ) throw std::runtime_error("ConstrL2LOrtho2 - not a line 2!");

	double er = a1x*a2x + a1y*a2y;

    if (var == l11x)
        return 2*er*(*l21x - *l22x);
    if (var == l11y)
        return 2*er*(*l21y - *l22y);
    if (var == l12x)
        return 2*er*(*l22x - *l21x);
    if (var == l12y)
        return 2*er*(*l22y - *l21y);
    if (var == l21x)
        return 2*er*(*l11x - *l12x);
    if (var == l21y)
        return 2*er*(*l11y - *l12y);
    if (var == l22x)
        return 2*er*(*l12x - *l11x);
    if (var == l22y)
        return 2*er*(*l12y - *l11y);
    return 0;
}


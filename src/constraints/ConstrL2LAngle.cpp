#include "ConstrL2LAngle.h"
#include <cmath>
#include <stdexcept>

georis::ConstrL2LAngle::ConstrL2LAngle(const georis::line2r &l1, const georis::line2r &l2,double *angle):ConstrL2L(l1,l2){
    if (angle == 0) throw std::invalid_argument("ConstrL2LAngle::invalid angle");
	_angle = angle;
}
double georis::ConstrL2LAngle::error()const{
    double a1x = *l12x - *l11x;
    double a1y = *l12y - *l11y;
    double a2x = *l22x - *l21x;
    double a2y = *l22y - *l21y;

	if ( a1x*a1x + a1y*a1y < epsi*epsi ) throw std::runtime_error("ConstrOrtoL::not a line 1!");
	if ( a2x*a2x + a2y*a2y < epsi*epsi ) throw std::runtime_error("ConstrOrtoL::not a line 2!");
	double s = a1x*a2x + a1y*a2y;
	double p = a1x*a2y - a1y*a2x;

	double er = atan(p/s) - *_angle;

    return er*er;
}
double georis::ConstrL2LAngle::grad(const double *var)const{
    double a1x = *l12x - *l11x;
    double a1y = *l12y - *l11y;
    double a2x = *l22x - *l21x;
    double a2y = *l22y - *l21y;

	if ( a1x*a1x + a1y*a1y < epsi*epsi ) throw std::runtime_error("ConstrOrtoL::not a line 1!");
	if ( a2x*a2x + a2y*a2y < epsi*epsi ) throw std::runtime_error("ConstrOrtoL::not a line 2!");
	double s = a1x*a2x + a1y*a2y;
	double p = a1x*a2y - a1y*a2x;

	double er = atan(p/s) - *_angle;

    if (var == l11x)
        return 2*er*((*l21y - *l22y)*s - (*l21x - *l22x)*p)/(s*s + p*p);
    if (var == l11y)
        return 2*er*((*l22x - *l21x)*s - (*l21y - *l22y)*p)/(s*s + p*p);
    if (var == l12x)
        return 2*er*((*l22y - *l21y)*s - (*l22x - *l21x)*p)/(s*s + p*p);
    if (var == l12y)
        return 2*er*((*l21x - *l22x)*s - (*l22y - *l21y)*p)/(s*s + p*p);
    if (var == l21x)
        return 2*er*((*l21y - *l11y)*s - (*l11x - *l12x)*p)/(s*s + p*p);
    if (var == l21y)
        return 2*er*((*l11x - *l21x)*s - (*l11y - *l12y)*p)/(s*s + p*p);
    if (var == l22x)
        return 2*er*((*l11y - *l12y)*s - (*l12x - *l11x)*p)/(s*s + p*p);
    if (var == l22y)
        return 2*er*((*l12x - *l11x)*s - (*l12y - *l11y)*p)/(s*s + p*p);
    return 0;
}


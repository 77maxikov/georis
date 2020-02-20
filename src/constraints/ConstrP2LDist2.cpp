#include "ConstrP2LDist2.h"
#include <cmath>
#include <stdexcept>

georis::ConstrP2LDist2::ConstrP2LDist2(const georis::point2r &p, const georis::line2r &l,double *dist){
	px = p.x;
	py = p.y;
	l1x = l.beg.x;
	l1y = l.beg.y;
	l2x = l.end.x;
	l2y = l.end.y;
	_dist = dist;
}
double georis::ConstrP2LDist2::error()const{
	double ax = *l2x - *l1x;
	double ay = *l2y - *l1y;
	double lpx = *px - *l1x;
	double lpy = *py - *l1y;

	if ( (ax*ax+ay*ay)<epsi ) throw std::runtime_error("ConstrP2LDist2::not a line!");
	double er = (lpx*ay-lpy*ax)/std::sqrt(ax*ax+ay*ay)-*_dist;
	return er*er;
}
double georis::ConstrP2LDist2::grad(const double *var)const{
	double ax = *l2x - *l1x;
	double ay = *l2y - *l1y;
	double lpx = *px - *l1x;
	double lpy = *py - *l1y;
    double d = std::sqrt(ax*ax+ay*ay);
    double S = lpx*ay-lpy*ax;

	if ( d < epsi ) throw std::runtime_error("ConstrP2LDist2::not a line!");

	double er = S/d-*_dist;
	if (var == px)
        return 2*er*ay/d;
    if (var == py)
        return 2*er*(-ax)/d;
    if (var == l1x)
        return 2*er*(( -*l2y + *py)*d - S*( -*l2x + *l1x)/d)/d/d;
    if (var == l1y)
        return 2*er*(( -*px + *l2x)*d - S*( -*l2y + *l1y)/d)/d/d;
    if (var == l2x)
        return 2*er*(( -*py + *l1y)*d - S*( *l2x - *l1x)/d)/d/d;
    if (var == l2y)
        return 2*er*(( *px - *l1x)*d - S*( *l2y - *l1y)/d)/d/d;
	return 0;
}

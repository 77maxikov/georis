#include "ConstrP2LDistAbs.h"
#include <cmath>
#include <stdexcept>

georis::ConstrP2LDistAbs::ConstrP2LDistAbs(const georis::point2r &p, const georis::line2r &l,double *dist){
	px = p.x;
	py = p.y;
	l1x = l.beg.x;
	l1y = l.beg.y;
	l2x = l.end.x;
	l2y = l.end.y;
	_dist = dist;
}
double georis::ConstrP2LDistAbs::error()const{
	double ax = *l2x - *l1x;
	double ay = *l2y - *l1y;
	double lpx = *px - *l1x;
	double lpy = *py - *l1y;
    double d = std::sqrt(ax*ax+ay*ay);
	if ( d < epsi ) throw std::runtime_error("ConstrP2LDistAbs::not a line!");
	double er = (lpx*ay-lpy*ax)/d-*_dist;
	return std::abs(er);
}
double georis::ConstrP2LDistAbs::grad(const double *var)const{
	double ax = *l2x - *l1x;
	double ay = *l2y - *l1y;
	double lpx = *px - *l1x;
	double lpy = *py - *l1y;
    double d = std::sqrt(ax*ax+ay*ay);
    if ( d < epsi ) throw std::runtime_error("ConstrP2LDistAbs::not a line!");
    double S = lpx*ay-lpy*ax;

    double w = (S/d - *_dist)<0?-1:1;

    if (var == px)
        return w*ay/d;
    if (var == py)
        return w*(-ax)/d;
    if (var == l1x)
        return w*(( -*l2y + *py)*d - S*( -*l2x + *l1x)/d)/d/d;
    if (var == l1y)
        return w*(( -*px + *l2x)*d - S*( -*l2y + *l1y)/d)/d/d;
    if (var == l2x)
        return w*(( -*py + *l1y)*d - S*( *l2x - *l1x)/d)/d/d;
    if (var == l2y)
        return w*(( *px - *l1x)*d - S*( *l2y - *l1y)/d)/d/d;
	return 0;
}

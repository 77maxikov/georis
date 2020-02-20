#include "ConstrP2PDist.h"
#include <cmath>
#include <stdexcept>

georis::ConstrP2PDist::ConstrP2PDist(const georis::point2r &p1,const georis::point2r &p2,double *dist){
	 _p1x = p1.x;
	 _p1y = p1.y;
    _p2x = p2.x;
    _p2y = p2.y;
    _dist = dist;
}
double georis::ConstrP2PDist::d2()const{
	return (*_p1x -*_p2x)*(*_p1x -*_p2x) + (*_p1y -*_p2y)*(*_p1y -*_p2y);
}
double georis::ConstrP2PDist::error()const{
	return std::sqrt(d2()) - *_dist;
}
double georis::ConstrP2PDist::grad(const double*var)const{
	double d =  std::sqrt(d2());
	if (d < epsi) throw std::runtime_error("ConstrP2PDist::points too close");
	if (var == _p1x)
		return (*_p1x - *_p2x)/d;
	if (var == _p1y)
		return (*_p1y - *_p2y)/d;
	if (var == _p2x)
		return (*_p2x - *_p1x)/d;
	if (var == _p2y)
		return (*_p2y - *_p1y)/d;
	return 0;
}

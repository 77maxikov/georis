#include "ConstrP2PDistAbs.h"
#include <cmath>
#include <stdexcept>
#include <iostream>

georis::ConstrP2PDistAbs::ConstrP2PDistAbs(const georis::point2r &p1,const georis::point2r &p2,double *dist){
	 _p1x = p1.x;
	 _p1y = p1.y;
    _p2x = p2.x;
    _p2y = p2.y;
    _dist = dist;
}
double georis::ConstrP2PDistAbs::d2()const{
	return (*_p1x -*_p2x)*(*_p1x -*_p2x) + (*_p1y -*_p2y)*(*_p1y -*_p2y);
}
double georis::ConstrP2PDistAbs::error()const{
	double er =  std::sqrt(d2()) - *_dist ;
	return std::abs(er);

}
double georis::ConstrP2PDistAbs::grad(const double*var)const{
	double d =  std::sqrt(d2());
	if (d < epsi) throw std::runtime_error("ConstrP2PDistAbs::points too close");

	double w = (d - *_dist)<0?-1/d:1/d;

	if (var == _p1x)
		return w*(*_p1x - *_p2x);
	if (var == _p1y)
		return w*(*_p1y - *_p2y);
	if (var == _p2x)
		return w*(*_p2x - *_p1x);
	if (var == _p2y)
		return w*(*_p2y - *_p1y);
	return 0;
}


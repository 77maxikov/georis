#include "ConstrP2PDist.h"
#include <cmath>
#include <stdexcept>

georis::ConstrP2PDist::ConstrP2PDist(const georis::point2r &p1,const georis::point2r &p2,paramProxy *dist){
	 _p1x = p1.x;
	 _p1y = p1.y;
    _p2x = p2.x;
    _p2y = p2.y;
    _dist = dist;
}
double georis::ConstrP2PDist::d2()const{
    return (*_p1x->pval -*_p2x->pval)*(*_p1x->pval -*_p2x->pval) + (*_p1y->pval -*_p2y->pval)*(*_p1y->pval -*_p2y->pval);
}
double georis::ConstrP2PDist::error()const{
    return std::sqrt(d2()) - *_dist->pval;
}

double georis::ConstrP2PDist::grad(const paramProxy *var)const{
	double d =  std::sqrt(d2());
//	if (d < epsi) throw std::runtime_error("ConstrP2PDist::points too close");
	if (var == _p1x)
        return (*_p1x->pval - *_p2x->pval)/d;
	if (var == _p1y)
        return (*_p1y->pval - *_p2y->pval)/d;
	if (var == _p2x)
        return (*_p2x->pval - *_p1x->pval)/d;
	if (var == _p2y)
        return (*_p2y->pval - *_p1y->pval)/d;
	return 0;
}

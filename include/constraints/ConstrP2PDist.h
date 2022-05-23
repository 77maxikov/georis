#ifndef _CONSTRP2PDIST_H
#define _CONSTRP2PDIST_H
#include "IConstraint.h"
#include "objects.h"


namespace georis{
// Distance between 2 points
class ConstrP2PDist:public IConstraint{
    paramProxy *_p1x,*_p1y;
    paramProxy *_p2x,*_p2y;
    paramProxy *_dist;
	 double d2()const;
public:
    ConstrP2PDist(const point2r&,const point2r&,paramProxy *dist);
    double error()const;
    DiDelegate grad(const paramProxy *);
    std::vector<paramProxy *> cparam()const { return {_p1x,_p1y,_p2x,_p2y}; };
    double dp1x(){ return (*_p1x->pval - *_p2x->pval)/std::sqrt(d2());}
    double dp1y(){ return (*_p1y->pval - *_p2y->pval)/std::sqrt(d2());}
    double dp2x(){ return (*_p2x->pval - *_p1x->pval)/std::sqrt(d2());}
    double dp2y(){ return (*_p2y->pval - *_p1y->pval)/std::sqrt(d2());}

};
}
#endif // _CONSTRP2PDIST_H

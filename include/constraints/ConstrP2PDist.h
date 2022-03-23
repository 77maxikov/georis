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
    double grad(const paramProxy *)const;
    std::vector<paramProxy *> cparam()const { return {_p1x,_p1y,_p2x,_p2y}; };
};
}
#endif // _CONSTRP2PDIST_H

#ifndef _CONSTRP2LDIST_H
#define _CONSTRP2LDIST_H
#include "IConstraint.h"
#include "objects.h"

// Point to line distance
namespace georis{
class ConstrP2LDist:public IConstraint{
	double *px,*py;
	double *l1x,*l1y,*l2x,*l2y;
	double *_dist;
	bool _varDist;
public:
	ConstrP2LDist(const point2r&, const line2r&,double *dist, bool varDist = false);
	double error()const;
	double grad(const double *)const;
};

class ConstrP2LaPEq:public IConstraint{
    double *p1x,*p1y;
    double *p2x,*p2y;
    double *l1x,*l1y,*l2x,*l2y;
public:
    ConstrP2LaPEq(const point2r&, const line2r&,const point2r&);
    double error()const;
    double grad(const double *)const;
};

class ConstrPP2LDist:public IConstraint{
    double *px1,*py1;
    double *px2,*py2;
    double *l1x,*l1y,*l2x,*l2y;
public:
    ConstrPP2LDist(const point2r&, const line2r&,const point2r&);
    double error()const;
    double grad(const double *)const;
};

}
#endif // _CONSTRP2LDIST_H

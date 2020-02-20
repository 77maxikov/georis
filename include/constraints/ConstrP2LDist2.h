#ifndef _CONSTRP2LDIST2_H
#define _CONSTRP2LDIST2_H
#include "IConstraint.h"
#include "objects.h"

// Point to line distance squared
namespace georis{
class ConstrP2LDist2:public IConstraint{
	double *px,*py;
	double *l1x,*l1y,*l2x,*l2y;
	double *_dist;
public:
	ConstrP2LDist2(const point2r&, const line2r&,double *dist);
	double error()const;
	double grad(const double *)const;
};
}
#endif // _CONSTRP2LDIST2_H

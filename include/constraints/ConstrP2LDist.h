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
}
#endif // _CONSTRP2LDIST_H

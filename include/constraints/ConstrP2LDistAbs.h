#ifndef _CONSTRP2LDISTABS_H
#define _CONSTRP2LDISTABS_H
#include "IConstraint.h"
#include "objects.h"

// Point to line distance
namespace georis{
class ConstrP2LDistAbs:public IConstraint{
	double *px,*py;
	double *l1x,*l1y,*l2x,*l2y;
	double *_dist;
public:
	ConstrP2LDistAbs(const point2r&, const line2r&,double *dist);
	double error()const;
	double grad(const double *)const;
};
}
#endif // _CONSTRP2LDISTABS_H

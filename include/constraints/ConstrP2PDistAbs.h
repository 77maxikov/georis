#ifndef _CONSTRP2PDISTABS_H
#define _CONSTRP2PDISTABS_H
#include "IConstraint.h"
#include "objects.h"

// Distance between 2 points
namespace georis{
class ConstrP2PDistAbs:public IConstraint{
    double *_p1x,*_p1y;
    double *_p2x,*_p2y;
    double *_dist;
	 double d2()const;
public:
    ConstrP2PDistAbs(const point2r&,const point2r&,double *dist);
    double error()const;
    double grad(const double *)const;
};
}
#endif // _CONSTRP2PDISTABS_H

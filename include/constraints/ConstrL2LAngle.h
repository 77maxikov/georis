#ifndef _CONSTRL2LANGLE_H
#define _CONSTRL2LANGLE_H
#include "ConstrL2L.h"
#include "IConstraint.h"
#include "objects.h"

namespace georis{
class ConstrL2LAngle:public ConstrL2L, public IConstraint{
	double *_angle;
public:
    ConstrL2LAngle(const line2r&, const line2r&,double *angle);
	double error()const;
	double grad(const double *v)const;
};
}
#endif // _CONSTRL2LANGLE_H

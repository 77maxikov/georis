#ifndef _CONSTRL2LANGLE_H
#define _CONSTRL2LANGLE_H
#include "ConstrL2L.h"
#include "IConstraint.h"
#include "objects.h"

namespace georis{
class ConstrL2LAngle:public ConstrL2L, public IConstraint{
    paramProxy *_angle;
public:
    ConstrL2LAngle(const line2r&, const line2r&,paramProxy *angle);
	double error()const;
    double grad(const paramProxy *v)const;
    std::vector<paramProxy*> cparam()const{return {l11x,l11y,l12x,l12y,l21x,l21y,l22x,l22y};}

};
}
#endif // _CONSTRL2LANGLE_H

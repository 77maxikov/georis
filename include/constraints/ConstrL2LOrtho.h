#ifndef _CONSTRL2LORTHO_H
#define _CONSTRL2LORTHO_H
#include "ConstrL2L.h"
#include "IConstraint.h"

namespace georis{
class ConstrL2LOrtho:public ConstrL2L,public IConstraint{
public:
	ConstrL2LOrtho(const line2r&l1, const line2r&l2):ConstrL2L(l1,l2){};
	ConstrL2LOrtho(const point2r&beg, const point2r&end, const line2r&l2):ConstrL2L(beg,end,l2){};
	double error()const;
	double grad(const double *v)const;
};
}
#endif // _CONSTRL2LORTHO_H

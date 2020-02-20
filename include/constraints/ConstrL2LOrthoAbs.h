#ifndef _CONSTRL2LORTHOABS_H
#define _CONSTRL2LORTHOABS_H
#include "ConstrL2L.h"
#include "IConstraint.h"

namespace georis{
class ConstrL2LOrthoAbs:public ConstrL2L,public IConstraint{
public:
	ConstrL2LOrthoAbs(const line2r&l1, const line2r&l2):ConstrL2L(l1,l2){};
	double error()const;
	double grad(const double *v)const;
};
}
#endif // CONSTRL2LORTHOABS_H

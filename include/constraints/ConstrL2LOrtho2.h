#ifndef _CONSTRL2LORTHO2_H
#define _CONSTRL2LORTHO2_H
#include "ConstrL2L.h"
#include "IConstraint.h"
namespace georis{
class ConstrL2LOrtho2:public ConstrL2L,public IConstraint{
public:
	ConstrL2LOrtho2(const line2r&l1, const line2r&l2):ConstrL2L(l1,l2){};
	double error()const;
	double grad(const double *v)const;
};
}
#endif // _CONSTRL2LORTHO2_H

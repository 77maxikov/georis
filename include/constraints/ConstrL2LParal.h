#ifndef _CONSTRL2LPARAL_H
#define _CONSTRL2LPARAL_H
#include "ConstrL2L.h"
#include "IConstraint.h"


namespace georis{
class ConstrL2LParal:public ConstrL2L,public IConstraint{
public:
    ConstrL2LParal(const line2r&l1, const line2r&l2):ConstrL2L(l1,l2){};
	double error()const;
    double grad(const double *v)const;
    std::vector<double*> cparam()const{return {l11x,l11y,l12x,l12y,l21x,l21y,l22x,l22y};}


};
}
#endif // _CONSTRL2LPARAL_H

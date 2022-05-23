#ifndef _CONSTRL2LPARAL_H
#define _CONSTRL2LPARAL_H
#include "ConstrL2L.h"
#include "IConstraint.h"


namespace georis{
class ConstrL2LParal:public ConstrL2L,public IConstraint{
public:
    ConstrL2LParal(const line2r&l1, const line2r&l2):ConstrL2L(l1,l2){};
	double error()const;
    DiDelegate grad(const paramProxy *v);
    std::vector<paramProxy*> cparam()const{return {l11x,l11y,l12x,l12y,l21x,l21y,l22x,l22y};}
    double dl11x(){ return -a2y; }
    double dl11y(){ return a2x; }
    double dl12x(){ return a2y; }
    double dl12y(){ return -a2x; }
    double dl21x(){ return a1y; }
    double dl21y(){ return -a1x; }
    double dl22x(){ return -a1y; }
    double dl22y(){ return a1x; }
};
}
#endif // _CONSTRL2LPARAL_H

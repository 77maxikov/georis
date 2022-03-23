#ifndef _CONSTRL2LORTHO2_H
#define _CONSTRL2LORTHO2_H
#include "ConstrL2L.h"
#include "IConstraint.h"

namespace georis{
class ConstrL2LOrtho2:public ConstrL2L,public IConstraint{
public:
    ConstrL2LOrtho2(const line2r&l1, const line2r&l2):ConstrL2L(l1,l2){};
    ConstrL2LOrtho2(const point2r&beg, const point2r&end, const line2r&l2):ConstrL2L(beg,end,l2){};
    double error()const;
    double grad(const paramProxy *v)const;
    std::vector<paramProxy*> cparam()const{return {l11x,l11y,l12x,l12y,l21x,l21y,l22x,l22y}; }
};
}
#endif // _CONSTRL2LORTHO_H

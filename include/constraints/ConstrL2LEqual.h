#ifndef _CONSTRL2LEQUAL_H
#define _CONSTRL2LEQUAL_H
#include "ConstrL2L.h"
#include "IConstraint.h"


namespace georis{
class ConstrL2LEqual:public ConstrL2L,public IConstraint{
public:
    ConstrL2LEqual(const line2r&l1, const line2r&l2):ConstrL2L(l1,l2){}
    ConstrL2LEqual(const point2r&pb1, const point2r&pe1,const point2r&pb2, const point2r&pe2):ConstrL2L(line2r(pb1,pe1),line2r(pb2,pe2)){}
	double error()const;
    double grad(const paramProxy *v)const;
    std::vector<paramProxy*> cparam()const{return {l11x,l11y,l12x,l12y,l21x,l21y,l22x,l22y};}

};
}
#endif // _CONSTRL2LEQUAL_H

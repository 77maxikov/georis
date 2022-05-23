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
    DiDelegate grad(const paramProxy *v);
    std::vector<paramProxy*> cparam()const{return {l11x,l11y,l12x,l12y,l21x,l21y,l22x,l22y};}

    double dl11x(){
        prepare();
        return -2*a1x;
    }
    double dl11y(){
        prepare();
        return -2*a1y;
    }
    double dl12x(){
        prepare();
        return 2*a1x;
    }
    double dl12y(){
        prepare();
        return 2*a1y;
    }
    double dl21x(){
        prepare();
        return 2*a2x;
    }
    double dl21y(){
        prepare();
        return 2*a2y;
    }
    double dl22x(){
        prepare();
        return -2*a2x;
    }
    double dl22y(){
        prepare();
        return -2*a2y;
    }


private:
    mutable double a1x, a1y, a2x,a2y;
    void prepare()const{
        a1x = *l12x->pval - *l11x->pval;
        a1y = *l12y->pval - *l11y->pval;
        a2x = *l22x->pval - *l21x->pval;
        a2y = *l22y->pval - *l21y->pval;

        if ( a1x*a1x + a1y*a1y < epsi*epsi ) throw std::runtime_error("ConstrL2LEqual::not a line 1!");
        if ( a2x*a2x + a2y*a2y < epsi*epsi ) throw std::runtime_error("ConstrL2LEqual::not a line 2!");
    }
};
}
#endif // _CONSTRL2LEQUAL_H

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
    DiDelegate grad(const paramProxy *v);
    std::vector<paramProxy*> cparam()const{return {l11x,l11y,l12x,l12y,l21x,l21y,l22x,l22y}; }

    double dl11x(){
        prepare();
        return -a2x/Q1/Q2 - S*(-a1x/Q1)/Q1/Q2/Q1;
    }
    double dl11y(){
        prepare();
        return -a2y/Q1/Q2 - S*(-a1y/Q1)/Q1/Q2/Q1;
    }
    double dl12x(){
        prepare();
        return a2x/Q1/Q2 - S*(a1x/Q1)/Q1/Q2/Q1;
    }
    double dl12y(){
        prepare();
        return a2y/Q1/Q2 - S*(a1y/Q1)/Q1/Q2/Q1;
    }
    double dl21x(){
        prepare();
        return -a1x/Q1/Q2 + S*a2x/Q2/Q2/Q1/Q2;
    }
    double dl21y(){
        prepare();
        return -a1y/Q1/Q2 + S*a2y/Q2/Q2/Q1/Q2;
    }
    double dl22x(){
        prepare();
        return a1x/Q1/Q2 - S*a2x/Q2/Q2/Q1/Q2;
    }
    double dl22y(){
        prepare();
        return a1y/Q1/Q2 - S*a2y/Q2/Q2/Q1/Q2;
    }
private:
    mutable double S, Q1,Q2;
    void prepare()const{
        ConstrL2L::prepare();
        S = a1x*a2x + a1y*a2y;
        Q1 = sqrt(a1x*a1x+a1y*a1y);
        Q2 = sqrt(a2x*a2x+a2y*a2y);
    }
};
}
#endif // _CONSTRL2LORTHO_H

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
    DiDelegate grad(const paramProxy *v);
    std::vector<paramProxy*> cparam()const{return {l11x,l11y,l12x,l12y,l21x,l21y,l22x,l22y};}

    double dl11x(){
        prepare();
        return (-a2y*c + a2x*s)/(s*s + c*c);
    }
    double dl11y(){
        prepare();
        return (a2x*c + a2y*s)/(s*s + c*c);
    }
    double dl12x(){
        prepare();
        return (a2y*c - a2x*s)/(s*s + c*c);
    }
    double dl12y(){
        prepare();
        return (-a2x*c - a2y*s)/(s*s + c*c);
    }
    double dl21x(){
        prepare();
        return (a1y*c + a1x*s)/(s*s + c*c);
    }
    double dl21y(){
        prepare();
        return (a1x*c + a1y*s)/(s*s + c*c);
    }
    double dl22x(){
        prepare();
        return (-a1y*c - a1x*s)/(s*s + c*c);
    }
    double dl22y(){
        prepare();
        return (a1x*c - a1y*s)/(s*s + c*c);
    }
private:
    mutable double a1x,a1y,a2x,a2y;
    mutable double c,s;
    void prepare()const {
        a1x = *l12x->pval - *l11x->pval;
        a1y = *l12y->pval - *l11y->pval;
        a2x = *l22x->pval - *l21x->pval;
        a2y = *l22y->pval - *l21y->pval;

        if ( a1x*a1x + a1y*a1y < epsi*epsi ) throw std::runtime_error("ConstrL2LAngle::not a line 1!");
        if ( a2x*a2x + a2y*a2y < epsi*epsi ) throw std::runtime_error("ConstrL2LAngle::not a line 2!");

        // Check if we have common point for lines
        if ( l11x->pval == l22x->pval && l11y->pval == l22y->pval){
            a2x *= -1; a2y *= -1;
        }
        if ( l12x->pval == l21x->pval && l12y->pval == l21y->pval){
            a1x *= -1; a1y *= -1;
        }
        c = a1x*a2x + a1y*a2y;
        s = a1x*a2y - a1y*a2x;
    }

};
}
#endif // _CONSTRL2LANGLE_H

#ifndef _CONSTRPP2LSYM_H
#define _CONSTRPP2LSYM_H
#include "IConstraint.h"
#include "ConstrL2L.h"

#include "objects.h"


namespace georis{
// Points symmetric to line
class ConstrPP2LSym:public ConstrL2L,public IConstraint{
public:
    ConstrPP2LSym(const point2r&beg,const point2r&end,const line2r&l2):ConstrL2L(beg,end,l2){};
    double error()const;
    DiDelegate grad(const paramProxy *);
    std::vector<paramProxy *> cparam()const { return {l11x,l11y,l12x,l12y,l21x,l21y,l22x,l22y}; };

    double dl11x(){prepare();return 0.5*ay/d;}
    double dl11y(){prepare();return 0.5*(-ax)/d;}
    double dl12x(){prepare();return 0.5*( ay )/d;}
    double dl12y(){prepare();return 0.5*(-ax)/d;}
    double dl21x(){prepare();return (-*l22y->pval + mpy)/d + (ay*(mpx - *l21x->pval)- ax *(mpy - *l21y->pval))/d/d/d*ax;}
    double dl21y(){prepare();return (-mpx + *l22x->pval)/d + (ay*(mpx - *l21x->pval)-ax*(mpy-*l21y->pval))/d/d/d*ay;}
    double dl22x(){prepare();return (-mpy + *l21y->pval)/d - (ay*(mpx-*l21x->pval)-ax*(mpy-*l21y->pval))/d/d/d*ax;}
    double dl22y(){prepare();return (mpx - *l21x->pval)/d - (ay*(mpx-*l21x->pval)-ax*(mpy-*l21y->pval))/d/d/d*ay;}
private:
    mutable double ax, ay, mpx,mpy,d;
    void prepare()const{
        ax = *l22x->pval - *l21x->pval;
        ay = *l22y->pval - *l21y->pval;
        mpx = (*l12x->pval + *l11x->pval)/2;
        mpy = (*l12y->pval + *l11y->pval)/2;
        d = sqrt(ax*ax + ay*ay);
    }
};
}
#endif // _CONSTRPP2LSYM_H

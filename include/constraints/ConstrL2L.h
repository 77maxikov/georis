#ifndef _CONSTRL2L_H
#define _CONSTRL2L_H
#include "objects.h"
namespace georis{
class ConstrL2L{
public:
    ConstrL2L(const line2r&, const line2r&);
    ConstrL2L(const point2r&,const point2r&, const line2r&);
protected:
    paramProxy *l11x,*l11y,*l12x,*l12y;
    paramProxy *l21x,*l21y,*l22x,*l22y;

    mutable double a1x, a1y, a2x, a2y;

    void prepare()const{
        a1x = *l12x->pval - *l11x->pval;
        a1y = *l12y->pval - *l11y->pval;
        a2x = *l22x->pval - *l21x->pval;
        a2y = *l22y->pval - *l21y->pval;
    }

};
}
#endif // _CONSTRL2L_H

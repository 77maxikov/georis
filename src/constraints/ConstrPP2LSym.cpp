#include "ConstrPP2LSym.h"
#include <cmath>
#include <stdexcept>


double georis::ConstrPP2LSym::error()const {
    double ax = *l22x->pval - *l21x->pval;
    double ay = *l22y->pval - *l21y->pval;
    double mpx = (*l12x->pval + *l11x->pval)/2;
    double mpy = (*l12y->pval + *l11y->pval)/2;
    return (ay*(mpx - *l21x->pval) - ax*(mpy - *l21y->pval))/sqrt(ax*ax+ay*ay);
}
georis::DiDelegate georis::ConstrPP2LSym::grad(const paramProxy *var) {
    if (var == l11x)
        return georis::DiDelegate::from_method<georis::ConstrPP2LSym,&georis::ConstrPP2LSym::dl11x>(this);
    if (var == l11y)
        return georis::DiDelegate::from_method<georis::ConstrPP2LSym,&georis::ConstrPP2LSym::dl11y>(this);
    if (var == l12x)
        return georis::DiDelegate::from_method<georis::ConstrPP2LSym,&georis::ConstrPP2LSym::dl12x>(this);
    if (var == l12y)
        return georis::DiDelegate::from_method<georis::ConstrPP2LSym,&georis::ConstrPP2LSym::dl12y>(this);
    if (var == l21x)
        return georis::DiDelegate::from_method<georis::ConstrPP2LSym,&georis::ConstrPP2LSym::dl21x>(this);
    if (var == l21y)
        return georis::DiDelegate::from_method<georis::ConstrPP2LSym,&georis::ConstrPP2LSym::dl21y>(this);
    if (var == l22x)
        return georis::DiDelegate::from_method<georis::ConstrPP2LSym,&georis::ConstrPP2LSym::dl22x>(this);
    if (var == l22y)
        return georis::DiDelegate::from_method<georis::ConstrPP2LSym,&georis::ConstrPP2LSym::dl22y>(this);
    return georis::DiDelegate();
}


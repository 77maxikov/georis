#include "ConstrL2LOrtho2.h"
#include <cmath>
#include <stdexcept>

double georis::ConstrL2LOrtho2::error()const{
    ConstrL2L::prepare();
    return (a1x*a2x + a1y*a2y)/sqrt(a1x*a1x+a1y*a1y)/sqrt(a2x*a2x+a2y*a2y);
}
georis::DiDelegate georis::ConstrL2LOrtho2::grad(const paramProxy *var){
    prepare();

    if (var == l11x)
        return georis::DiDelegate::from_method<georis::ConstrL2LOrtho2,&georis::ConstrL2LOrtho2::dl11x>(this);
    if (var == l11y)
        return georis::DiDelegate::from_method<georis::ConstrL2LOrtho2,&georis::ConstrL2LOrtho2::dl11y>(this);
    if (var == l12x)
        return georis::DiDelegate::from_method<georis::ConstrL2LOrtho2,&georis::ConstrL2LOrtho2::dl12x>(this);
    if (var == l12y)
        return georis::DiDelegate::from_method<georis::ConstrL2LOrtho2,&georis::ConstrL2LOrtho2::dl12y>(this);
    if (var == l21x)
        return georis::DiDelegate::from_method<georis::ConstrL2LOrtho2,&georis::ConstrL2LOrtho2::dl21x>(this);
    if (var == l21y)
        return georis::DiDelegate::from_method<georis::ConstrL2LOrtho2,&georis::ConstrL2LOrtho2::dl21y>(this);
    if (var == l22x)
        return georis::DiDelegate::from_method<georis::ConstrL2LOrtho2,&georis::ConstrL2LOrtho2::dl22x>(this);
    if (var == l22y)
        return georis::DiDelegate::from_method<georis::ConstrL2LOrtho2,&georis::ConstrL2LOrtho2::dl22y>(this);
    return georis::DiDelegate();
}


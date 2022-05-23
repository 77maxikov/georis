#include "ConstrL2LEqual.h"

double georis::ConstrL2LEqual::error()const {
    prepare();
    return a1x*a1x + a1y*a1y - a2x*a2x - a2y*a2y;
}
georis::DiDelegate georis::ConstrL2LEqual::grad(const paramProxy *var){

    if (var == l11x)
        return georis::DiDelegate::from_method<georis::ConstrL2LEqual,&georis::ConstrL2LEqual::dl11x>(this);
    if (var == l11y)
        return georis::DiDelegate::from_method<georis::ConstrL2LEqual,&georis::ConstrL2LEqual::dl11y>(this);
    if (var == l12x)
        return georis::DiDelegate::from_method<georis::ConstrL2LEqual,&georis::ConstrL2LEqual::dl12x>(this);
    if (var == l12y)
        return georis::DiDelegate::from_method<georis::ConstrL2LEqual,&georis::ConstrL2LEqual::dl12y>(this);
    if (var == l21x)
        return georis::DiDelegate::from_method<georis::ConstrL2LEqual,&georis::ConstrL2LEqual::dl21x>(this);
    if (var == l21y)
        return georis::DiDelegate::from_method<georis::ConstrL2LEqual,&georis::ConstrL2LEqual::dl21y>(this);
    if (var == l22x)
        return georis::DiDelegate::from_method<georis::ConstrL2LEqual,&georis::ConstrL2LEqual::dl22x>(this);
    if (var == l22y)
        return georis::DiDelegate::from_method<georis::ConstrL2LEqual,&georis::ConstrL2LEqual::dl22y>(this);
    return DiDelegate();
}


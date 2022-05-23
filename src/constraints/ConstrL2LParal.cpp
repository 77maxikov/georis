#include "ConstrL2LParal.h"
#include <cmath>
#include <stdexcept>

double georis::ConstrL2LParal::error()const{
    ConstrL2L::prepare();

    return  a1x*a2y - a1y*a2x;
}
georis::DiDelegate georis::ConstrL2LParal::grad(const paramProxy *var){
    ConstrL2L::prepare();
    if (var == l11x)
        return georis::DiDelegate::from_method<georis::ConstrL2LParal,&georis::ConstrL2LParal::dl11x>(this);
    if (var == l11y)
        return georis::DiDelegate::from_method<georis::ConstrL2LParal,&georis::ConstrL2LParal::dl11y>(this);
    if (var == l12x)
        return georis::DiDelegate::from_method<georis::ConstrL2LParal,&georis::ConstrL2LParal::dl12x>(this);
    if (var == l12y)
        return georis::DiDelegate::from_method<georis::ConstrL2LParal,&georis::ConstrL2LParal::dl12y>(this);
    if (var == l21x)
        return georis::DiDelegate::from_method<georis::ConstrL2LParal,&georis::ConstrL2LParal::dl21x>(this);
    if (var == l21y)
        return georis::DiDelegate::from_method<georis::ConstrL2LParal,&georis::ConstrL2LParal::dl21y>(this);
    if (var == l22x)
        return georis::DiDelegate::from_method<georis::ConstrL2LParal,&georis::ConstrL2LParal::dl22x>(this);
    if (var == l22y)
        return georis::DiDelegate::from_method<georis::ConstrL2LParal,&georis::ConstrL2LParal::dl22y>(this);

    return georis::DiDelegate();
}


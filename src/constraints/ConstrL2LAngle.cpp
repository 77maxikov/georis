#include "ConstrL2LAngle.h"
#include <cmath>
#include <stdexcept>
#include "common_math.h"

georis::ConstrL2LAngle::ConstrL2LAngle(const georis::line2r &l1, const georis::line2r &l2,paramProxy *angle):ConstrL2L(l1,l2){
    if (angle == 0) throw std::invalid_argument("ConstrL2LAngle::invalid angle");
    _angle = angle;
}
double georis::ConstrL2LAngle::error()const{
    prepare();
    return atan2(s,c) - deg2rad(*_angle->pval);
}
georis::DiDelegate georis::ConstrL2LAngle::grad(const paramProxy *var){
    if (var == l11x)
        return georis::DiDelegate::from_method<georis::ConstrL2LAngle,&georis::ConstrL2LAngle::dl11x>(this);
    if (var == l11y)
        return georis::DiDelegate::from_method<georis::ConstrL2LAngle,&georis::ConstrL2LAngle::dl11y>(this);
    if (var == l12x)
        return georis::DiDelegate::from_method<georis::ConstrL2LAngle,&georis::ConstrL2LAngle::dl12x>(this);
    if (var == l12y)
        return georis::DiDelegate::from_method<georis::ConstrL2LAngle,&georis::ConstrL2LAngle::dl12y>(this);
    if (var == l21x)
        return georis::DiDelegate::from_method<georis::ConstrL2LAngle,&georis::ConstrL2LAngle::dl21x>(this);
    if (var == l21y)
        return georis::DiDelegate::from_method<georis::ConstrL2LAngle,&georis::ConstrL2LAngle::dl21y>(this);
    if (var == l22x)
        return georis::DiDelegate::from_method<georis::ConstrL2LAngle,&georis::ConstrL2LAngle::dl22x>(this);
    if (var == l22y)
        return georis::DiDelegate::from_method<georis::ConstrL2LAngle,&georis::ConstrL2LAngle::dl22y>(this);
    return georis::DiDelegate();
}


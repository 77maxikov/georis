#include "ConstrPP2LSym.h"
#include <cmath>
#include <stdexcept>


double georis::ConstrPP2LSym::error()const {
    double ax = *l22x - *l21x;
    double ay = *l22y - *l21y;
    double mpx = (*l12x + *l11x)/2;
    double mpy = (*l12y + *l11y)/2;
    return (ay*(mpx - *l21x) - ax*(mpy - *l21y))/sqrt(ax*ax+ay*ay);
}
double georis::ConstrPP2LSym::grad(const double *var)const {
    double ax = *l22x - *l21x;
    double ay = *l22y - *l21y;
    double ppx = *l12x - *l11x;
    double ppy = *l12y - *l11y;
    double mpx = (*l12x + *l11x)/2;
    double mpy = (*l12y + *l11y)/2;
    double d = sqrt(ax*ax + ay*ay);

    if (var == l11x){
        //return 0.5*ay - ax;
        return 0.5*(*l22y - *l21y)/d;
    }
    if (var == l11y){
        //return -0.5*ax - ay;
        return 0.5*(-*l22x + *l21x)/d;
    }
    if (var == l12x){
        //return 0.5*ay + ax;
        return 0.5*( *l22y - *l21y)/d;
    }
    if (var == l12y){
        //return -0.5*ax + ay;
        return 0.5*(-*l22x + *l21x)/d;
    }
    if (var == l21x){
        //return -ay + mpy - *l21y - ppx;
        return (-*l22y + mpy)/d + (ay*(mpx - *l21x)- ax *(mpy - *l21y))/d/d/d*ax;

    }
    if (var == l21y){
        //return -mpx + *l21x + ax - ppy;
        return (-mpx + *l22x)/d + (ay*(mpx - *l21x)-ax*(mpy-*l21y))/d/d/d*ay;

    }
    if (var == l22x){
        //return -mpy + *l21y + ppx;
        return (-mpy + *l21y)/d - (ay*(mpx-*l21x)-ax*(mpy-*l21y))/d/d/d*ax;
    }
    if (var == l22y){
        //return mpx - *l21x + ppy;
        return (mpx - *l21x)/d - (ay*(mpx-*l21x)-ax*(mpy-*l21y))/d/d/d*ay;
    }

    return 0;
}


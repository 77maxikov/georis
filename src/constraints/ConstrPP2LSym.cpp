#include "ConstrPP2LSym.h"
#include <cmath>
#include <stdexcept>


double georis::ConstrPP2LSym::error()const {
    double ax = *l22x - *l21x;
    double ay = *l22y - *l21y;
    double mpx = (*l12x + *l11x)/2;
    double mpy = (*l12y + *l11y)/2;
    return ay*(mpx - *l21x) - ax*(mpy - *l21y);
}
double georis::ConstrPP2LSym::grad(const double *var)const {
    double ax = *l22x - *l21x;
    double ay = *l22y - *l21y;
    double ppx = *l12x - *l11x;
    double ppy = *l12y - *l11y;
    double mpx = (*l12x + *l11x)/2;
    double mpy = (*l12y + *l11y)/2;

    if (var == l11x){
        return 0.5*ay - ax;
    }
    if (var == l11y){
        return -0.5*ax - ay;
    }
    if (var == l12x){
        return 0.5*ay + ax;
    }
    if (var == l12y){
        return -0.5*ax + ay;
    }
    if (var == l21x){
        return -ay + mpy - *l21y - ppx;
    }
    if (var == l21y){
        return -mpx + *l21x + ax - ppy;
    }
    if (var == l22x){
        return -mpy + *l21y + ppx;
    }
    if (var == l22y){
        return mpx - *l21x + ppy;
    }

    return 0;
}


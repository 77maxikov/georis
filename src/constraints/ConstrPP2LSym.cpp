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
double georis::ConstrPP2LSym::grad(const paramProxy *var)const {
    double ax = *l22x->pval - *l21x->pval;
    double ay = *l22y->pval - *l21y->pval;
    //double ppx = *l12x->pval - *l11x->pval;
    //double ppy = *l12y->pval - *l11y->pval;
    double mpx = (*l12x->pval + *l11x->pval)/2;
    double mpy = (*l12y->pval + *l11y->pval)/2;
    double d = sqrt(ax*ax + ay*ay);

    if (var == l11x){
        //return 0.5*ay - ax;
        return 0.5*ay/d;
    }
    if (var == l11y){
        //return -0.5*ax - ay;
        return 0.5*(-ax)/d;
    }
    if (var == l12x){
        //return 0.5*ay + ax;
        return 0.5*( ay )/d;
    }
    if (var == l12y){
        //return -0.5*ax + ay;
        return 0.5*(-ax)/d;
    }
    if (var == l21x){
        //return -ay + mpy - *l21y - ppx;
        return (-*l22y->pval + mpy)/d + (ay*(mpx - *l21x->pval)- ax *(mpy - *l21y->pval))/d/d/d*ax;

    }
    if (var == l21y){
        //return -mpx + *l21x + ax - ppy;
        return (-mpx + *l22x->pval)/d + (ay*(mpx - *l21x->pval)-ax*(mpy-*l21y->pval))/d/d/d*ay;

    }
    if (var == l22x){
        //return -mpy + *l21y + ppx;
        return (-mpy + *l21y->pval)/d - (ay*(mpx-*l21x->pval)-ax*(mpy-*l21y->pval))/d/d/d*ax;
    }
    if (var == l22y){
        //return mpx - *l21x + ppy;
        return (mpx - *l21x->pval)/d - (ay*(mpx-*l21x->pval)-ax*(mpy-*l21y->pval))/d/d/d*ay;
    }

    return 0;
}


#include "ConstrL2LParal.h"
#include <cmath>
#include <stdexcept>

double georis::ConstrL2LParal::error()const{
    double a1x = *l12x->pval - *l11x->pval;
    double a1y = *l12y->pval - *l11y->pval;
    double a2x = *l22x->pval - *l21x->pval;
    double a2y = *l22y->pval - *l21y->pval;

	if ( a1x*a1x + a1y*a1y < epsi*epsi ) throw std::runtime_error("ConstrL2LParal::not a line 1!");
	if ( a2x*a2x + a2y*a2y < epsi*epsi ) throw std::runtime_error("ConstrL2LParal::not a line 2!");

    return  a1x*a2y - a1y*a2x;
}
double georis::ConstrL2LParal::grad(const paramProxy *var)const{
    double a1x = *l12x->pval - *l11x->pval;
    double a1y = *l12y->pval - *l11y->pval;
    double a2x = *l22x->pval - *l21x->pval;
    double a2y = *l22y->pval - *l21y->pval;

	if ( a1x*a1x + a1y*a1y < epsi*epsi ) throw std::runtime_error("ConstrL2LParal::not a line 1!");
	if ( a2x*a2x + a2y*a2y < epsi*epsi ) throw std::runtime_error("ConstrL2LParal::not a line 2!");

    if (var == l11x)
        return (-a2y);
    if (var == l11y)
        return a2x;
    if (var == l12x)
        return a2y;
    if (var == l12y)
        return (-a2x);
    if (var == l21x)
        return a1y;
    if (var == l21y)
        return (-a1x);
    if (var == l22x)
        return (-a1y);
    if (var == l22y)
        return a1x;
    return 0;
}


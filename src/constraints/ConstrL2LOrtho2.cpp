#include "ConstrL2LOrtho2.h"
#include <cmath>
#include <stdexcept>

double georis::ConstrL2LOrtho2::error()const{
    double a1x = *l12x->pval - *l11x->pval;
    double a1y = *l12y->pval - *l11y->pval;
    double a2x = *l22x->pval - *l21x->pval;
    double a2y = *l22y->pval - *l21y->pval;

    //if ( a1x*a1x + a1y*a1y < epsi*epsi ) throw std::runtime_error("ConstrL2LOrtho2 - not a line 1!");
    //if ( a2x*a2x + a2y*a2y < epsi*epsi ) throw std::runtime_error("ConstrL2LOrtho2 - not a line 2!");
    return (a1x*a2x + a1y*a2y)/sqrt(a1x*a1x+a1y*a1y)/sqrt(a2x*a2x+a2y*a2y);
}
double georis::ConstrL2LOrtho2::grad(const paramProxy *var)const{
    double a1x = *l12x->pval - *l11x->pval;
    double a1y = *l12y->pval - *l11y->pval;
    double a2x = *l22x->pval - *l21x->pval;
    double a2y = *l22y->pval - *l21y->pval;

    //if ( a1x*a1x + a1y*a1y < epsi*epsi ) throw std::runtime_error("ConstrL2LOrtho::grad2 - not a line 1!");
    //if ( a2x*a2x + a2y*a2y < epsi*epsi ) throw std::runtime_error("ConstrL2LOrtho::grad2 - not a line 2!");
    double S = a1x*a2x + a1y*a2y;
    double dS = 0;
    double Q1 = sqrt(a1x*a1x+a1y*a1y);
    double Q2 = sqrt(a2x*a2x+a2y*a2y);
    double dQ1 = 0;
    double dQ2 = 0;
    if (var == l11x){
        dS = -a2x;
        dQ1 = -a1x/Q1;
    }
    if (var == l11y){
        dS = -a2y;
        dQ1 = -a1y/Q1;
    }
    if (var == l12x){
        dS = a2x;
        dQ1 = a1x/Q1;
    }
    if (var == l12y){
        dS = a2y;
	dQ1 = a1y/Q1;
    }
    if (var == l21x){
	dS = -a1x;
	dQ2 = -a2x/Q2;
}
    if (var == l21y){
	dS = -a1y;
	dQ2 = -a2y/Q2;
}
    if (var == l22x){
        dS = a1x;
	dQ2 = a2x/Q2;
}
    if (var == l22y){
	dS = a1y;
	dQ2 = a2y/Q2;        
}


    return dS/Q1/Q2 - S*(dQ1*Q2 + Q1*dQ2)/Q1/Q2/Q1/Q2;
}


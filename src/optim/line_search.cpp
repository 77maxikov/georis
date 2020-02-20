#include "line_search.h"
#include <iostream>
// GolderRule search

void lineSearchGolden(const v_type &dir,const MinTaskN &task,v_type &x, unsigned & fevals) {

    const double tau = 2/(1+std::sqrt(5.0));
    // Bracketing step
    v_type x1(x);
    s_type fx1 = (*task.target)(x1); ++fevals;
    v_type x2(x1+tau*dir);
    s_type fx2 = (*task.target)(x2);	++fevals;
//std::cout << "fx2 = " << fx2 << std::endl;
    while (fx2 < fx1) {
        x2 = x1+1/tau*(x2-x1);
        fx2 = (*task.target)(x2); ++fevals;
//std::cout << "fx2 = " << fx2 << std::endl;
    }

    v_type x3 = x2 - tau*(x2-x1);
    s_type fx3 = (*task.target)(x3); ++fevals;
    v_type x4 = x1 + tau*(x2-x1);
    s_type fx4 = (*task.target)(x4); ++fevals;
//std::cout << "fx3 = " << fx3 << std::endl;
//std::cout << "fx4 = " << fx4 << std::endl;
    // Localization step
    unsigned flag = 0;
    while (1) {
        if ( norm2(x1-x2) < task.stopcond.tolx ) {
            flag |= OPTFLAG_TOLX;
            break;
        }
        if ( std::abs(fx1-fx2) < task.stopcond.tolf ) {
//std::cout << "LineSearchGolden: fx1 = " << fx1 << " fx2 = " << fx2 << " tolf = " << std::abs(fx1-fx2) << " task.stopcond.tolf = "<< task.stopcond.tolf <<  std::endl;
            flag |= OPTFLAG_TOLF;
            break;
        }
        if ( fevals > task.stopcond.fevals) {
            flag |= OPTFLAG_FEVALS;
            break;
        }
        if (fx3 < fx4) {
//std::cout << "shrink " << std::endl;
            x2 = x4;
            fx2 = fx4;
            x4 = x3;
            fx4 = fx3;
            x3 = x2 - tau*(x2-x1);
            fx3 = (*task.target)(x3);
            ++fevals;
        } else {
            x1 = x3;
            fx1 = fx3;
            x3 = x4;
            fx3 = fx4;
            x4 = x1 + tau*(x2-x1);
            fx4 = (*task.target)(x4);
            ++fevals;
        }
//std::cout << "LineSearchGolden: fx3  = " << fx3 << std::endl;
    }
    if ( fx4 < fx1 ) {
        x = x4;
        fx1 = fx4;
    }
    if ( fx3 < fx1 ) {
        x = x3;
    }
}

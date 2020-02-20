#ifndef _CONSTREQUAL_H
#define _CONSTREQUAL_H
#include "IConstraint.h"
namespace georis{
class ConstrEqual:public IConstraint{
    double *_p1,*_p2;
public:
    ConstrEqual(double *p1,double *p2):_p1(p1),_p2(p2){};
    double error()const;
    double grad(const double *)const;
};
}
#endif // _CONSTREQUAL_H

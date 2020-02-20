#ifndef _CONSTREQUALABS_H
#define _CONSTREQUALABS_H
#include "IConstraint.h"
namespace georis{
class ConstrEqualAbs:public IConstraint{
    double *_p1,*_p2;
public:
    ConstrEqualAbs(double *p1,double *p2):_p1(p1),_p2(p2){};
    double error()const;
    double grad(const double *)const;
};
}
#endif // _CONSTREQUALABS_H

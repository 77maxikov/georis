#ifndef _CONSTREQUAL_H
#define _CONSTREQUAL_H
#include "IConstraint.h"
namespace georis{
// Equal values
class ConstrEqual:public IConstraint{
    double *_p1,*_p2;
public:
    ConstrEqual(double *p1,double *p2):_p1(p1),_p2(p2){};
    double error()const;
    double grad(const double *)const;
    std::vector<double*> cparam()const{return {_p1,_p2};}
};
// Mid value
class ConstrMidVal:public IConstraint{
    double *_mid,*_p1,*_p2;
public:
    ConstrMidVal(double *mid, double *p1,double *p2):_mid(mid),_p1(p1),_p2(p2){};
    double error()const;
    double grad(const double *)const;
    std::vector<double*> cparam()const{return {_mid,_p1,_p2};}
};
}
#endif // _CONSTREQUAL_H

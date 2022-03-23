#ifndef _CONSTREQUAL_H
#define _CONSTREQUAL_H
#include "IConstraint.h"
namespace georis{
// Equal values
class ConstrEqual:public IConstraint{
    paramProxy *_p1,*_p2;
public:
    ConstrEqual(paramProxy *p1,paramProxy *p2):_p1(p1),_p2(p2){};
    double error()const;
    double grad(const paramProxy *)const;
    std::vector<paramProxy*> cparam()const{return {_p1,_p2};}
};
// Mid value
class ConstrMidVal:public IConstraint{
    paramProxy *_mid,*_p1,*_p2;
public:
    ConstrMidVal(paramProxy *mid, paramProxy *p1,paramProxy *p2):_mid(mid),_p1(p1),_p2(p2){};
    double error()const;
    double grad(const paramProxy *)const;
    std::vector<paramProxy*> cparam()const{return {_mid,_p1,_p2};}
};
}
#endif // _CONSTREQUAL_H

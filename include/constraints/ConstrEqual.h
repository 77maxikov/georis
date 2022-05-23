#ifndef _CONSTREQUAL_H
#define _CONSTREQUAL_H
#include "IConstraint.h"
namespace georis{

class ConstrConst:public IConstraint{
    paramProxy *_p1;
public:
    ConstrConst(paramProxy *p1):_p1(p1){};
    double error()const{return 0;};
    DiDelegate grad(const paramProxy *){return DiDelegate();};
    std::vector<paramProxy*> cparam()const{return {_p1};}
};

// Equal values
class ConstrEqual:public IConstraint{
    paramProxy *_p1,*_p2;
public:
    ConstrEqual(paramProxy *p1,paramProxy *p2):_p1(p1),_p2(p2){};
    double error()const;
    DiDelegate grad(const paramProxy *);
    std::vector<paramProxy*> cparam()const{return {_p1,_p2};}

    constexpr double dp1(){ return 1; }
    double dp2(){ return -1; }
};
// Mid value
class ConstrMidVal:public IConstraint{
    paramProxy *_mid,*_p1,*_p2;
public:
    ConstrMidVal(paramProxy *mid, paramProxy *p1,paramProxy *p2):_mid(mid),_p1(p1),_p2(p2){};
    double error()const;
    DiDelegate grad(const paramProxy *);
    std::vector<paramProxy*> cparam()const{return {_mid,_p1,_p2};}

    constexpr double dmid(){return 2.0;}
    constexpr double dp(){return -1.0;}

};
}
#endif // _CONSTREQUAL_H

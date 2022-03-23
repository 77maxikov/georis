#ifndef _CONSTRP2LDIST_H
#define _CONSTRP2LDIST_H
#include "IConstraint.h"
#include "objects.h"

namespace georis{
// Point on line
class ConstrPOnL:public IConstraint{
    paramProxy *_px,*_py;
    paramProxy *_l1x,*_l1y,*_l2x,*_l2y;
public:
    ConstrPOnL(const point2r&, const line2r&);
    double error()const;
    double grad(const paramProxy *)const;
    std::vector<paramProxy*> cparam()const{return {_px,_py,_l1x,_l1y,_l2x,_l2y};}
};

// Point to line distance
class ConstrP2LDist:public IConstraint{
    paramProxy *_px,*_py;
    paramProxy *_l1x,*_l1y,*_l2x,*_l2y;
    paramProxy *_dist;
	bool _varDist;
public:
    ConstrP2LDist(const point2r&, const line2r&,paramProxy *dist, bool varDist = false);
	double error()const;
    double grad(const paramProxy *)const;
    std::vector<paramProxy*> cparam()const{return {_px,_py,_l1x,_l1y,_l2x,_l2y,_dist};}
};

/*
class ConstrP2LaPEq:public IConstraint{
    double *p1x,*p1y;
    double *p2x,*p2y;
    double *l1x,*l1y,*l2x,*l2y;
public:
    ConstrP2LaPEq(const point2r&, const line2r&,const point2r&);
    double error()const;
    double grad(const double *)const;
};


class ConstrPP2LDist:public IConstraint{
    double *px1,*py1;
    double *px2,*py2;
    double *l1x,*l1y,*l2x,*l2y;
public:
    ConstrPP2LDist(const point2r&, const line2r&,const point2r&);
    double error()const;
    double grad(const double *)const;
};
*/
class ConstrP2PLDist:public IConstraint{
    paramProxy *_p1x,*_p1y;
    paramProxy *_p2x,*_p2y;
    paramProxy *_l1x,*_l1y,*_l2x,*_l2y;
public:
    ConstrP2PLDist(const point2r&, const point2r&,const line2r&);
    double error()const;
    double grad(const paramProxy *)const;
    std::vector<paramProxy*> cparam()const{return {_p1x,_p1y,_p2x,_p2y,_l1x,_l1y,_l2x,_l2y};}
};

}
#endif // _CONSTRP2LDIST_H

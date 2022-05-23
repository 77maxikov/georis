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
    DiDelegate grad(const paramProxy *);
    std::vector<paramProxy*> cparam()const{return {_px,_py,_l1x,_l1y,_l2x,_l2y};}

    double dpx() { prepare();return ay/d; }
    double dpy() { prepare();return (-ax)/d; }
    double dl1x(){ prepare();return (( -*_l2y->pval + *_py->pval)*d - S*( -*_l2x->pval + *_l1x->pval)/d)/d/d; }
    double dl1y(){ prepare();return (( -*_px->pval + *_l2x->pval)*d + S*ay/d)/d/d; }
    double dl2x(){ prepare();return (-lpy*d - S*ax/d)/d/d; }
    double dl2y(){ prepare();return (lpx*d - S*ay/d)/d/d; }
private:
    mutable double ax, ay, lpx, lpy, d, S;
    void prepare()const{
        ax = *_l2x->pval - *_l1x->pval;
        ay = *_l2y->pval - *_l1y->pval;
        lpx = *_px->pval - *_l1x->pval;
        lpy = *_py->pval - *_l1y->pval;
        d = std::sqrt(ax*ax+ay*ay);
        if ( d < epsi ) throw std::runtime_error("ConstrPOnL::not a line!");
        S = lpx*ay-lpy*ax;
    }
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
    DiDelegate grad(const paramProxy *);
    std::vector<paramProxy*> cparam()const{return {_px,_py,_l1x,_l1y,_l2x,_l2y,_dist};}

    double dpx() { prepare();return w*ay/d; }
    double dpy() { prepare();return w*(-ax)/d; }
    double dl1x(){ prepare();return w*(( -*_l2y->pval + *_py->pval)*d + S*ax/d)/d/d; }
    double dl1y(){ prepare();return w*(( -*_px->pval + *_l2x->pval)*d - S*(-ay)/d)/d/d; }
    double dl2x(){ prepare();return w*(( -lpy)*d - S*ax/d)/d/d; }
    double dl2y(){ prepare();return w*(lpx*d - S*ay/d)/d/d; }
    double ddist(){return -1.0;}
private:
    mutable double ax,ay, lpx, lpy, d, S, w;

    void prepare()const{
        ax = *_l2x->pval - *_l1x->pval;
        ay = *_l2y->pval - *_l1y->pval;
        lpx = *_px->pval - *_l1x->pval;
        lpy = *_py->pval - *_l1y->pval;
        d = std::sqrt(ax*ax+ay*ay);
        if ( d < epsi ) throw std::runtime_error("ConstrP2LDist::not a line!");
        S = lpx*ay-lpy*ax;
        w = S<0?-1:1;
    }
};

class ConstrP2PLDist:public IConstraint{
    paramProxy *_p1x,*_p1y;
    paramProxy *_p2x,*_p2y;
    paramProxy *_l1x,*_l1y,*_l2x,*_l2y;
public:
    ConstrP2PLDist(const point2r&, const point2r&,const line2r&);
    double error()const;
    DiDelegate grad(const paramProxy *);
    std::vector<paramProxy*> cparam()const{return {_p1x,_p1y,_p2x,_p2y,_l1x,_l1y,_l2x,_l2y};}

    double dp1x(){
        return 1/(sqrt(*_p2x->pval**_p2x->pval-2.0**_p2x->pval**_p1x->pval+*_p1x->pval**_p1x->pval+*_p2y->pval**_p2y->pval-2.0**_p2y->pval**_p1y->pval+*_p1y->pval**_p1y->pval))*(-2.0**_p2x->pval+2.0**_p1x->pval)/2.0-2.0*((*_p1x->pval-*_l1x->pval)*(*_l2y->pval-*_l1y->pval)-(*_p1y->pval-*_l1y->pval)*(*_l2x->pval-*_l1x->pval))/(*_l2x->pval**_l2x->pval-2.0**_l2x->pval**_l1x->pval+*_l1x->pval**_l1x->pval+*_l2y->pval**_l2y->pval-2.0**_l2y->pval**_l1y->pval+*_l1y->pval**_l1y->pval)*(*_l2y->pval-*_l1y->pval);
    }
    double dp1y(){
        return 1/(sqrt(*_p2x->pval**_p2x->pval-2.0**_p2x->pval**_p1x->pval+*_p1x->pval**_p1x->pval+*_p2y->pval**_p2y->pval-2.0**_p2y->pval**_p1y->pval+*_p1y->pval**_p1y->pval))*(-2.0**_p2y->pval+2.0**_p1y->pval)/2.0-2.0*((*_p1x->pval-*_l1x->pval)*(*_l2y->pval-*_l1y->pval)-(*_p1y->pval-*_l1y->pval)*(*_l2x->pval-*_l1x->pval))/(*_l2x->pval**_l2x->pval-2.0**_l2x->pval**_l1x->pval+*_l1x->pval**_l1x->pval+*_l2y->pval**_l2y->pval-2.0**_l2y->pval**_l1y->pval+*_l1y->pval**_l1y->pval)*(-*_l2x->pval+*_l1x->pval);
    }
    double dp2x(){
        return (*_p2x->pval-*_p1x->pval)/sqrt(*_p2x->pval**_p2x->pval-2.0**_p2x->pval**_p1x->pval+*_p1x->pval**_p1x->pval+*_p2y->pval**_p2y->pval-2.0**_p2y->pval**_p1y->pval+*_p1y->pval**_p1y->pval);
    }
    double dp2y(){
        return (*_p2y->pval-*_p1y->pval)/sqrt(*_p2x->pval**_p2x->pval-2.0**_p2x->pval**_p1x->pval+*_p1x->pval**_p1x->pval+*_p2y->pval**_p2y->pval-2.0**_p2y->pval**_p1y->pval+*_p1y->pval**_p1y->pval);
    }
    double dl1x(){
        return 2.0*(*_p1x->pval**_l2y->pval-*_p1x->pval**_l1y->pval-*_l1x->pval**_l2y->pval-*_p1y->pval**_l2x->pval+*_p1y->pval**_l1x->pval+*_l1y->pval**_l2x->pval)*(*_l2y->pval-*_l1y->pval)*(*_l2y->pval**_l2y->pval-*_p1y->pval**_l2y->pval-*_l2y->pval**_l1y->pval+*_p1y->pval**_l1y->pval+*_p1x->pval**_l1x->pval+*_l2x->pval**_l2x->pval-*_l2x->pval**_l1x->pval-*_p1x->pval**_l2x->pval)/pow(*_l2x->pval**_l2x->pval-2.0**_l2x->pval**_l1x->pval+*_l1x->pval**_l1x->pval+*_l2y->pval**_l2y->pval-2.0**_l2y->pval**_l1y->pval+*_l1y->pval**_l1y->pval,2.0);
    }
    double dl1y(){
        return -2.0*(*_p1x->pval**_l2y->pval-*_p1x->pval**_l1y->pval-*_l1x->pval**_l2y->pval-*_p1y->pval**_l2x->pval+*_p1y->pval**_l1x->pval+*_l1y->pval**_l2x->pval)*(*_l2x->pval-*_l1x->pval)*(*_l2y->pval**_l2y->pval-*_p1y->pval**_l2y->pval-*_l2y->pval**_l1y->pval+*_p1y->pval**_l1y->pval+*_p1x->pval**_l1x->pval+*_l2x->pval**_l2x->pval-*_l2x->pval**_l1x->pval-*_p1x->pval**_l2x->pval)/pow(*_l2x->pval**_l2x->pval-2.0**_l2x->pval**_l1x->pval+*_l1x->pval**_l1x->pval+*_l2y->pval**_l2y->pval-2.0**_l2y->pval**_l1y->pval+*_l1y->pval**_l1y->pval,2.0);
    }
    double dl2x(){
        return -2.0*(*_p1x->pval**_l2y->pval-*_p1x->pval**_l1y->pval-*_l1x->pval**_l2y->pval-*_p1y->pval**_l2x->pval+*_p1y->pval**_l1x->pval+*_l1y->pval**_l2x->pval)*(*_l2y->pval-*_l1y->pval)*(*_l2y->pval**_l1y->pval-*_p1y->pval**_l2y->pval+*_p1x->pval**_l1x->pval+*_p1y->pval**_l1y->pval-*_p1x->pval**_l2x->pval-*_l1x->pval**_l1x->pval-*_l1y->pval**_l1y->pval+*_l2x->pval**_l1x->pval)/pow(*_l2x->pval**_l2x->pval-2.0**_l2x->pval**_l1x->pval+*_l1x->pval**_l1x->pval+*_l2y->pval**_l2y->pval-2.0**_l2y->pval**_l1y->pval+*_l1y->pval**_l1y->pval,2.0);
    }
    double dl2y(){
        return 2.0*(*_p1x->pval**_l2y->pval-*_p1x->pval**_l1y->pval-*_l1x->pval**_l2y->pval-*_p1y->pval**_l2x->pval+*_p1y->pval**_l1x->pval+*_l1y->pval**_l2x->pval)*(*_l2x->pval-*_l1x->pval)*(*_l2y->pval**_l1y->pval-*_p1y->pval**_l2y->pval+*_p1x->pval**_l1x->pval+*_p1y->pval**_l1y->pval-*_p1x->pval**_l2x->pval-*_l1x->pval**_l1x->pval-*_l1y->pval**_l1y->pval+*_l2x->pval**_l1x->pval)/pow(*_l2x->pval**_l2x->pval-2.0**_l2x->pval**_l1x->pval+*_l1x->pval**_l1x->pval+*_l2y->pval**_l2y->pval-2.0**_l2y->pval**_l1y->pval+*_l1y->pval**_l1y->pval,2.0);
    }
};

}
#endif // _CONSTRP2LDIST_H

#pragma once
#include <cmath>
#include <vector>
#include "georis.h"

namespace georis{

struct point2r{
    double *x,*y;
    point2r(double *ar):x(ar),y(ar+1){}
    point2r(double *xx,double *yy):x(xx),y(yy){}
    point2r(const point2r &p):x(p.x),y(p.y){}
};

struct point3r{
    double *x,*y,*z;
    point3r(double *ar):x(ar),y(ar+1),z(ar+2){}
    point3r(double *xx,double *yy,double *zz):x(xx),y(yy),z(zz){}
    point3r(const point3r &p):x(p.x),y(p.y),z(p.z){}
};
struct vec3r{
    double *x,*y,*z;
    vec3r(double *ar):x(ar),y(ar+1),z(ar+2){}
    vec3r(double *xx,double *yy,double *zz):x(xx),y(yy),z(zz){}
    vec3r(const point3r &p):x(p.x),y(p.y),z(p.z){}
};


struct line2r{//
    point2r beg;
    point2r end;
    line2r(const point2r&b,const point2r &e):beg(b),end(e){}
};

struct plane{
    point3r x0;
    vec3r n;
    vec3r xdir;
    plane(const point3r &p,const vec3r &_n,const vec3r &xd):x0(p),n(_n),xdir(xd){}
};

struct sketch{
    plane* base;

};





/*
struct objInfo{
	ObjectType type;
	std::vector<double> param;
	unsigned status;
};
*/
struct sketchObj {
    bool fixed;
    sketchObj(){fixed = false;}
    virtual ~sketchObj() {}
    virtual double dist2point(double x,double y)const = 0;
    virtual bool inRect(double xtl,double ytl,double xbr,double ybr)const = 0;
    virtual ObjectType type()const = 0;
    virtual void move(double dx,double dy) = 0;

};
struct ptrep:public point2r, public sketchObj {
    ptrep(double *xx,double *yy):point2r(xx,yy) {}
    virtual void move(double dx,double dy) {
        *x += dx;
        *y += dy;        
    }
    double dist2point(double _x,double _y)const {
        return std::sqrt( (*x -_x)*(*x -_x) + (*y -_y)*(*y -_y) );
    }
    bool inRect(double xtl,double ytl,double xbr,double ybr)const{
            return ( xtl<= *x && *x <= xbr && ytl <= *y && *y <= ybr );
    }
    virtual ObjectType type()const {
        return ObjectType::OT_POINT;
    }
};
struct lirep:public sketchObj {
    ptrep *beg;
    ptrep *end;
    lirep(ptrep *b,ptrep *e):beg(b),end(e) {};
    operator line2r ()const {
        return line2r(*beg,*end);
    }
    virtual void move(double dx,double dy) {
        beg->move(dx,dy);
        end->move(dx,dy);
    }
    double dist2point(double _x,double _y)const {
        double ax = *(end->x) - *(beg->x);
        double ay = *(end->y) - *(beg->y);
        double lpx = _x - *(beg->x);
        double lpy = _y - *(beg->y);
        double lena = std::sqrt(ax*ax+ay*ay);
        double lproa = (lpx*ax+lpy*ay)/lena;
        double ptl = std::abs(lpx*ay-lpy*ax)/lena;

        if ( lproa < 0 ) return 2*std::sqrt(lpx*lpx+lpy*lpy);
        if ( std::abs(lproa) > lena ) return 2*(sqrt(ptl*ptl + (lproa-lena)*(lproa-lena)));

        return ptl;
    }
    bool inRect(double xtl,double ytl,double xbr,double ybr)const{
            return ( xtl<= *(beg->x) && *(beg->x) <= xbr && ytl <= *(beg->y) && *(beg->y) <= ybr &&
                                     xtl<= *(end->x) && *(end->x) <= xbr && ytl <= *(end->y) && *(end->y) <= ybr );
    }

    virtual ObjectType type()const {
        return ObjectType::OT_SEGMENT;
    }
};
struct circrep:public sketchObj {
    ptrep *center;
    double *r;
    circrep(ptrep *cent,double *rr):center(cent),r(rr) {};
    double dist2point(double _x,double _y) const {
        return std::abs( std::sqrt((*(center->x) -_x)*(*(center->x) -_x) + (*(center->y) -_y)*(*(center->y) -_y) ) - *r);
    }
    bool inRect(double xtl,double ytl,double xbr,double ybr)const{
            return ( xtl<= (*(center->x)-*r) && (*(center->x)+*r) <= xbr && ytl <= (*(center->y)-*r) && (*(center->y)+*r) <= ybr );
    }
            virtual void move(double dx,double dy){
                    center->move(dx,dy);
            }
    virtual ObjectType type()const {
        return ObjectType::OT_CIRCLE;
    }
};


class Controller;

}


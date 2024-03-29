#pragma once
#include <cmath>
#include <vector>
#include <limits>
#include <cassert>
#include "georis.h"
#include "param.h"

#include "mooLog.h"

namespace georis{

template<typename T>
struct point2_t{
    T x,y;
    point2_t(T *ar):x(ar[0]),y(ar[1]){}
    point2_t(T xx,T yy):x(xx),y(yy){}
    point2_t(const point2_t<T> &p):x(p.x),y(p.y){}
};



template<typename T>
struct point2ref_t{
    T *x,*y;
    point2ref_t(T *ar):x(ar),y(ar+1){}
    point2ref_t(T *xx,T *yy):x(xx),y(yy){}
    point2ref_t(const point2ref_t<T> &p):x(p.x),y(p.y){}
};

typedef point2ref_t<paramProxy> point2r ;

template<typename T>
struct point3t{
    T *x,*y,*z;
    point3t(T *ar):x(ar),y(ar+1),z(ar+2){}
    point3t(T *xx,T *yy,T *zz):x(xx),y(yy),z(zz){}
    point3t(const point3t<T> &p):x(p.x),y(p.y),z(p.z){}
};

typedef point3t<paramProxy> point3r;

template<typename T>
struct vec3r{
    T *x,*y,*z;
    vec3r(T *ar):x(ar),y(ar+1),z(ar+2){}
    vec3r(T *xx,T *yy,T *zz):x(xx),y(yy),z(zz){}
    vec3r(const point3t<T> &p):x(p.x),y(p.y),z(p.z){}
};

template<typename T>
struct line2t{//
    point2ref_t<T> beg;
    point2ref_t<T> end;
    line2t(const point2ref_t<T>&b,const point2ref_t<T> &e):beg(b),end(e){}
};

typedef line2t<paramProxy> line2r;

template<typename T>
struct plane{
    point3t<T> x0;
    vec3r<T> n;
    vec3r<T> xdir;
    plane(const point3t<T> &p,const vec3r<T> &_n,const vec3r<T> &xd):x0(p),n(_n),xdir(xd){}
};

struct sketch{
    plane<paramProxy>* base;

};

template<typename T> struct AABB_t {
    point2_t<T> BL,TR;
    AABB_t():BL(0,0),TR(0,0){}
    AABB_t(T xbl,T ybl,T xtr,T ytr):BL(xbl,ybl),TR(xtr,ytr) {}
/*
    AABB_t(const line2t<T>&segment) {
        BL.x = (std::min)(segment.beg.x,segment.end.x);
        BL.y = (std::min)(segment.beg.y,segment.end.y);
        TR.x = (std::max)(segment.beg.x,segment.end.x);
        TR.y = (std::max)(segment.beg.y,segment.end.y);
    }
*/
    T area()const {
        if (empty()) return -1;
        return (TR.x - BL.x)*(TR.y - BL.y);
    }
    T perimeter()const {
        if (empty()) return -1;
        return 2*((TR.lat - BL.lat) + (TR.lon - BL.lon));
    }
    bool empty()const {
        return BL.x >= TR.x && BL.y >= TR.y;
    }

    static AABB_t createEmpty() {
        return AABB_t(0,0,-1,-1);
    }
    // Union
    AABB_t operator+(const AABB_t<T>&b)const {
        AABB_t res;
        res.BL.x = (std::min)(BL.x,b.BL.x);
        res.BL.y = (std::min)(BL.y,b.BL.y);
        res.TR.x = (std::max)(TR.x,b.TR.x);
        res.TR.y = (std::max)(TR.y,b.TR.y);
        return res;
    }
    // Intersection
    AABB_t operator*(const AABB_t<T>&b)const {
        if (TR.x < b.BL.x ||
                b.TR.x < BL.x ||
                TR.y < b.BL.y ||
                b.TR.y < BL.y ) return createEmpty();
        AABB_t res;
        T tmp[4];
        tmp[0] = BL.x;
        tmp[1] = b.BL.x;
        tmp[2] = TR.x;
        tmp[3] = b.TR.x;
        for (size_t k = 0; k<3; ++k)
            for (size_t m = k+1; m<4; ++m)
                if (tmp[k] > tmp[m])
                    std::swap(tmp[k],tmp[m]);
        res.BL.x = tmp[1];
        res.TR.x = tmp[2];

        tmp[0] = BL.y;
        tmp[1] = b.BL.y;
        tmp[2] = TR.y;
        tmp[3] = b.TR.y;
        for (size_t k = 0; k<3; ++k)
            for (size_t m = k+1; m<4; ++m)
                if (tmp[k] > tmp[m])
                    std::swap(tmp[k],tmp[m]);
        res.BL.y = tmp[1];
        res.TR.y = tmp[2];

        return res;
    }
    bool contains(const point2_t<T>&test){
        return (BL.x <= test.x &&
                test.x <= TR.x &&
                BL.y <= test.y &&
                test.y <= TR.y);
    }
};
typedef AABB_t<double> AABBr;




/*
struct objInfo{
	ObjectType type;
	std::vector<double> paramProxy;
	unsigned status;
};
*/
struct sketchObj {
    bool fixed;
    sketchObj(){fixed = false;}
    virtual ~sketchObj() {}
    virtual double dist2point(double x,double y)const = 0;
    virtual bool inRect(double xtl,double ytl,double xbr,double ybr)const = 0;
    virtual operator AABBr()const = 0;
    virtual ObjectType type()const = 0;
    virtual void move(double dx,double dy) = 0;
    virtual void setParam(const std::vector<double>& parame) = 0;
    virtual void getParam(std::vector<double>& parame)const = 0;
    virtual void getParamInfo(std::vector<paramProxy*>& ppa)const = 0;

};
struct ptrep:public point2r, public sketchObj {
    ptrep(paramProxy *xx,paramProxy *yy):point2r(xx,yy) {}
    virtual void move(double dx,double dy) {
        *(x->pval) += dx;
        *(y->pval) += dy;
    }
    virtual double dist2point(double _x,double _y)const {
        // Halving the dist to boost points' priority
        return 0.5*std::sqrt( (*x->pval -_x)*(*x->pval -_x) + (*y->pval -_y)*(*y->pval -_y) );
    }
    virtual bool inRect(double minx,double miny,double maxx,double maxy)const{
            return ( minx<= *x->pval && *x->pval <= maxx && miny <= *y->pval && *y->pval <= maxy );
    }
    virtual ObjectType type()const {
        return ObjectType::OT_POINT;
    }
    virtual operator AABBr()const{
        return AABBr(*(x->pval),*(y->pval),*(x->pval),*(y->pval));
    }
    virtual void setParam(const std::vector<double>& parame){
        assert(parame.size() == 2);
        *(x->pval) = parame[0];
        *(y->pval) = parame[1];
    }
    virtual void getParam(std::vector<double>& parame)const{
        parame.clear();
        parame.push_back(*x->pval);
        parame.push_back(*y->pval);
    }
    virtual void getParamInfo(std::vector<paramProxy*>& ppa)const{
        ppa.clear();
        ppa.push_back(x);
        ppa.push_back(y);
    }
};
struct lirep:public sketchObj {
    ptrep *beg;
    ptrep *end;
    ObjectType ty;
    lirep(ptrep *b,ptrep *e,ObjectType lot):beg(b),end(e),ty(lot){
        if ( ty == OT_LINE || ty == OT_RAY || ty == OT_SEGMENT ) return;
        throw std::invalid_argument("lirep: bad line type");
    }
    operator line2r ()const {
        return line2r(*beg,*end);
    }
    virtual void move(double dx,double dy) {        
        if ( beg->x->pval == end->x->pval ){
            if ( !beg->fixed ) beg->move(dx,dy);
            if ( !end->fixed ) *(end->y->pval) += dy;
        }
        else if ( beg->y->pval == end->y->pval ){
            if ( !beg->fixed ) beg->move(dx,dy);
            if ( !end->fixed ) *(end->x->pval) += dx;
        }
        else{
            if ( !beg->fixed ) beg->move(dx,dy);
            if ( !end->fixed ) end->move(dx,dy);
        }
    }
    double dist2point(double _x,double _y)const {
        double ax = *(end->x->pval) - *(beg->x->pval);
        double ay = *(end->y->pval) - *(beg->y->pval);
        double lpx = _x - *(beg->x->pval);
        double lpy = _y - *(beg->y->pval);
        double lena = std::sqrt(ax*ax+ay*ay);
        double ptl = std::abs(lpx*ay-lpy*ax)/lena;
        if ( ty == OT_LINE )
            return ptl;

        double lproa = (lpx*ax+lpy*ay)/lena;

        if ( lproa < 0 ) return std::sqrt(lpx*lpx+lpy*lpy);

        if ( lproa > lena && ty == OT_SEGMENT ) return (sqrt(ptl*ptl + (lproa-lena)*(lproa-lena)));

        return ptl;
    }
    bool inRect(double xmin,double ymin,double xmax,double ymax)const{
        if ( ty == OT_SEGMENT ) return ( xmin<= *(beg->x->pval) && *(beg->x->pval) <= xmax && ymin <= *(beg->y->pval) && *(beg->y->pval) <= ymax &&
                                     xmin<= *(end->x->pval) && *(end->x->pval) <= xmax && ymin <= *(end->y->pval) && *(end->y->pval) <= ymax );
        return false;
    }

    virtual ObjectType type()const {
        return ty;
    }
    virtual operator AABBr()const{
        return AABBr(*(beg->x->pval),*(beg->y->pval),*(end->x->pval),*(end->y->pval));
    }
    virtual void setParam(const std::vector<double>& parame){
        assert(parame.size() == 4);
        *beg->x->pval = parame[0];
        *beg->y->pval = parame[1];
        *end->x->pval = parame[2];
        *end->y->pval = parame[3];
    }
    virtual void getParam(std::vector<double>& parame)const{
        parame.clear();
        parame.push_back(*beg->x->pval);
        parame.push_back(*beg->y->pval);
        parame.push_back(*end->x->pval);
        parame.push_back(*end->y->pval);
    }
    virtual void getParamInfo(std::vector<paramProxy*>& ppa)const{
        ppa.clear();
        ppa.push_back(beg->x);
        ppa.push_back(beg->y);
        ppa.push_back(end->x);
        ppa.push_back(end->y);
    }

};
struct circrep:public sketchObj {
    ptrep *center;
    paramProxy *r;
    circrep(ptrep *cent,paramProxy *rr):center(cent),r(rr) {};
    double dist2point(double _x,double _y) const {
        return std::abs( std::sqrt((*(center->x->pval) -_x)*(*(center->x->pval) -_x) + (*(center->y->pval) -_y)*(*(center->y->pval) -_y) ) - *r->pval);
    }
    bool inRect(double xmin,double ymin,double xmax,double ymax)const{
        return ( xmin<= (*(center->x->pval)-*r->pval) && (*(center->x->pval)+*r->pval) <= xmax && ymin <= (*(center->y->pval)-*r->pval) && (*(center->y->pval)+*r->pval) <= ymax );
    }
    virtual void move(double dx,double dy){
        center->move(dx,dy);
    }
    virtual ObjectType type()const {
        return ObjectType::OT_CIRCLE;
    }
    virtual operator AABBr()const{
        return AABBr(*(center->x->pval)-*(r->pval),*(center->y->pval)-*(r->pval),*(center->x->pval)+*(r->pval),*(center->y->pval)+*(r->pval));
    }
    virtual void setParam(const std::vector<double>& parame){
        assert(parame.size() == 3);
        *center->x->pval = parame[0];
        *center->y->pval = parame[1];
        *r->pval = parame[2];
    }
    virtual void getParam(std::vector<double>& parame)const{
        parame.clear();
        parame.push_back(*center->x->pval);
        parame.push_back(*center->y->pval);
        parame.push_back(*r->pval);
    }
    virtual void getParamInfo(std::vector<paramProxy*>& ppa)const{
        ppa.clear();
        ppa.push_back(center->x);
        ppa.push_back(center->y);
        ppa.push_back(r);
    }

};

struct arcrep:public sketchObj {
    ptrep *center;
    ptrep *beg;
    ptrep *end;

    arcrep(ptrep *cent,ptrep *be,ptrep *en):center(cent),beg(be),end(en) {}

    double dist2point(double x_,double y_) const {
        double dxb = *(beg->x->pval) - *(center->x->pval);
        double dyb = *(beg->y->pval) - *(center->y->pval);

        double dxe = *(end->x->pval) - *(center->x->pval);
        double dye = *(end->y->pval) - *(center->y->pval);

        double dx = x_ - *(center->x->pval);
        double dy = y_ - *(center->y->pval);

        double bxe = dxb*dye - dxe*dyb;
        double bxt = dxb*dy - dx*dyb;
        double txe = dx*dye - dxe*dy;

        if ( bxe < 0 ){
            if ( bxt < 0 && txe < 0 )
                return std::numeric_limits<double>::infinity();
            return std::abs( std::sqrt( dxb*dxb + dyb*dyb) - std::sqrt( dx*dx + dy*dy));
        }
        else{
            if ( bxt > 0 && txe > 0 )
                return std::abs( std::sqrt( dxb*dxb + dyb*dyb) - std::sqrt( dx*dx + dy*dy));
            return std::numeric_limits<double>::infinity();
        }
    }  

    bool inRect(double xmin,double ymin,double xmax,double ymax)const{
        double dxb = *(beg->x->pval) - *(center->x->pval);
        double dyb = *(beg->y->pval) - *(center->y->pval);
        double r = std::sqrt(dxb*dxb + dyb*dyb);
        // test if arc's circle fully contained in rect
        if ( xmin <= *(center->x->pval)-r &&
             *(center->x->pval)+r <= xmax &&
             *(center->y->pval)+r <= ymax &&
             ymin <= *(center->y->pval)-r )
            return true;
        // test if arc's circle fully out of rect
        if ( *(center->x->pval)+r < xmin ||
             xmax < *(center->x->pval)-r ||
             ymax < *(center->y->pval)-r ||
             *(center->y->pval)+r  < ymin )
            return false;

        double top = std::max( *(beg->y->pval),*(end->y->pval) ) ,
               bottom = std::min( *(beg->y->pval),*(end->y->pval) ),
               left = std::min( *(beg->x->pval),*(end->x->pval) ),
               right = std::max( *(beg->x->pval),*(end->x->pval) );

        // test if any of four points (N,W,S,E) are contained in arc
        if ( !std::isinf(dist2point(*(center->x->pval),*(center->y->pval)+r)) )
            top = *(center->y->pval)+r;
        if ( !std::isinf(dist2point(*(center->x->pval),*(center->y->pval)-r)) )
            bottom = *(center->y->pval)-r;
        if ( !std::isinf(dist2point(*(center->x->pval)-r,*(center->y->pval))) )
            left = *(center->x->pval)-r;
        if ( !std::isinf(dist2point(*(center->x->pval)+r,*(center->y->pval))) )
            right = *(center->x->pval)+r;

        return (xmin <= left && right <= xmax && top <= ymax && ymin <= bottom);
    }
    virtual void move(double dx,double dy){
        center->move(dx,dy);
    }
    virtual ObjectType type()const {
        return ObjectType::OT_ARC;
    }
    virtual operator AABBr()const{
        double dxb = *(beg->x->pval) - *(center->x->pval);
        double dyb = *(beg->y->pval) - *(center->y->pval);
        double r = std::sqrt(dxb*dxb + dyb*dyb);
        return AABBr(*(center->x->pval)-r,*(center->y->pval)-r,*(center->x->pval)+r,*(center->y->pval)+r);
    }
    virtual void setParam(const std::vector<double>& parame){
        assert(parame.size() == 6);
        *center->x->pval = parame[0];
        *center->y->pval = parame[1];
        *beg->x->pval = parame[2];
        *beg->y->pval = parame[3];
        *end->x->pval = parame[4];
        *end->y->pval = parame[5];
    }
    virtual void getParam(std::vector<double>& parame)const{
        parame.clear();
        parame.push_back(*center->x->pval);
        parame.push_back(*center->y->pval);
        parame.push_back(*beg->x->pval);
        parame.push_back(*beg->y->pval);
        parame.push_back(*end->x->pval);
        parame.push_back(*end->y->pval);
    }
    virtual void getParamInfo(std::vector<paramProxy*>& ppa)const{
        ppa.clear();
        ppa.push_back(center->x);
        ppa.push_back(center->y);
        ppa.push_back(beg->x);
        ppa.push_back(beg->y);
        ppa.push_back(end->x);
        ppa.push_back(end->y);
    }

    double angle(){
        double dxb = *(beg->x->pval) - *(center->x->pval);
        double dyb = *(beg->y->pval) - *(center->y->pval);
        double phib = atan2(dyb,dxb);
        double dxe = *(end->x->pval) - *(center->x->pval);
        double dye = *(end->y->pval) - *(center->y->pval);
        double phie = atan2(dye,dxe);
        double a = phib - phie;
        double cro = dxb*dye - dxe*dyb;
        if ( cro > 0 ){
            if ( a > M_PI )
                return a;
            else
                return a - M_PI;
        }
        else{
            if ( a > M_PI )
                return a - M_PI;
            else
                return a;

        }
    }    
};




class Controller;


}


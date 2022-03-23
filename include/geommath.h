#ifndef GEOMMATH_H
#define GEOMMATH_H
#include <cmath>

static const double epsi = 1e-8;

template<typename T> T ppdist2(T p1x,T p1y, T p2x, T p2y){
    return std::sqrt((p1x - p2x)*(p1x - p2x) + (p1y - p2y)*(p1y - p2y));
}

template<typename T> T psegdist2(T px, T py, T s1x, T s1y, T s2x, T s2y){
    T v1x = px - s1x;
    T v1y = py - s1y;
    T v2x = px - s2x;
    T v2y = py - s2y;
    T sx = s2x - s1x;
    T sy = s2y - s1y;
    T nx = s2y - s1y;
    T ny = s1x - s2x;
    T v1s = v1x*sx + v1y*sy;
    T v2s = v2x*sx + v2y*sy;
    if ( v1s*v2s < 0 )
        return std::abs(v1x*nx + v1y*ny)/sqrt(nx*nx + ny*ny);
    if ( v1s > 0 && v2s >= 0 )
        return ppdist2(px,py,s2x,s2y);
    return ppdist2(px,py,s1x,s1y);
}

template<typename T> bool segcross(T s1bx,T s1by,T s1ex, T s1ey,
                                   T s2bx,T s2by,T s2ex, T s2ey,
                                   T& cx, T& cy){
    // ACHTUNG !!! what if a1x == a1y == 0 or a2x == a2y == 0

    T a1x = s1ex - s1bx;
    T a1y = s1ey - s1by;
    T a2x = s2bx - s2ex;
    T a2y = s2by - s2ey;
    T drx = s2bx - s1bx;
    T dry = s2by - s1by;

    T D = a1x*a2y - a1y*a2x;
    T D1 = drx*a2y - dry*a2x;
    T D2 = a1x*dry - a1y*drx;
    if ( std::abs(D) < epsi ){        
        if ( std::abs(D1) < epsi && std::abs(D2) < epsi ){
            // Test if endpoints of s2 are on s1
            T t = 0;
            if ( std::abs(a1x) > epsi ){
                if ( 0 <= (t = drx/a1x) && t <= 1.0 ){
                    cx = s2bx;cy = s2by; return true;
                }
                if ( 0 <= (t = (s2ex-s1bx)/a1x) && t <= 1.0){
                    cx = s2ex;cy = s2ey; return true;
                }
            }
            if ( std::abs(a1y) > epsi ){
                if ( 0<= (t = dry/a1y) && t <= 1.0 ) {
                    cx = s2bx;cy = s2by; return true;
                }
                if ( 0 <= (t = (s2ey-s1by)/a1y) && t <= 1.0 ){
                    cx = s2ex;cy = s2ey; return true;
                }
            }
            if ( std::abs(a2x) > epsi ){
                if ( 0 <= (t = -drx/a2x) && t <= 1.0 ){
                    cx = s1bx;cy = s1by; return true;
                }
                if ( 0 <= (t = (s1ex-s2bx)/a2x) && t <= 1.0){
                    cx = s1ex;cy = s1ey; return true;
                }
            }
            if ( std::abs(a2y) > epsi ){
                if ( 0<= (t = -dry/a2y) && t <= 1.0 ) {
                    cx = s1bx;cy = s1by; return true;
                }
                if ( 0 <= (t = (s2ey-s1by)/a1y) && t <= 1.0 ){
                    cx = s1ex;cy = s1ey; return true;
                }
            }
            return false;
        }
        return false;
    }
    cx = s1bx + D1/D*(s1ex - s1bx);
    cy = s1by + D1/D*(s1ey - s1by);
    return true;
}



#endif // GEOMMATH_H

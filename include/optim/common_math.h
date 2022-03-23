#ifndef _COMMONMATH_H
#define _COMMONMATH_H
#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880
#endif


#define deg2rad(x) ((x)/180.0*M_PI)
#define rad2deg(x) ((x)/M_PI*180.0)

inline double sind(double x) {return sin(fmod((x),360.0) * M_PI / 180);}
inline double cosd(double x) {return cos(fmod((x),360.0) * M_PI / 180);}

inline bool rcomp(double x,double y,double eps = 1e-5){return fabs(x-y)<eps;}

inline double sign(double x){
    return x<0.0?-1.0:(x>0.0?1.0:0.0);
}

inline double fsign(double x){
	if (rcomp(x,0.0)) return 0.0;
	if (x<0.0) return (-1.0);
	if (x>0.0) return 1.0;
	return 0.0;	
}

inline double SAT2(double x, double xl,double xu)
{
	if (x < xl) return (xl);
	if (x > xu) return xu;
	return x;
}
inline double wrapTo360(double deg){
   while (deg < 0) deg += 360.0;
   while (deg > 360) deg -= 360.0;
   return deg;
}
inline double wrapTo180(double deg){
   while (deg < -180) deg += 360.0;
   while (deg > 180) deg -= 360.0;
   return deg;
}
template<typename T> T mixedprod(T a[],T b[],T c[]){
	return a[0]*b[1]*c[2] + a[1]*b[2]*c[0] + a[2]*b[0]*c[1] - a[2]*b[1]*c[0] - a[1]*b[0]*c[2] - a[0]*b[2]*c[1];
}
#endif

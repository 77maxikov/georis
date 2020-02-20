#ifndef __COMMONMATH
#define __COMMONMATH

#define MS2KN_RATIO (3600.0/1852.0)
#define KN2MS_RATIO (1852.0/3600.0)
#define MS2KN(x) (MS2KN_RATIO*(x))
#define KN2MS(x) (KN2MS_RATIO*(x))

inline bool rcomp(double x,double y,double eps = 1e-5){return fabs(x-y)<eps;}

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
#endif
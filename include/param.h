#ifndef PARAMPROXY_H
#define PARAMPROXY_H
namespace georis{
struct paramProxy{
    double *pval;
    double *orig;
    paramProxy(double *p):pval(p),orig(p){}
};
}
#endif // paramProxy_H

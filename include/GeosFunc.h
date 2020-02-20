#ifndef _GEOSFUNC_H
#define _GEOSFUNC_H
#include "GeosFuncN.h"

namespace georis{
class GeosFuncAdapter:public IOptFunc1_N {
    GeosFuncN* _funcN;
public:
    GeosFuncAdapter(GeosFuncN* funcN):_funcN(funcN) {}
    void add(IConstraint *c) {
        _funcN->add(c);
    };
    size_t inDim()const {
        return _funcN->inDim();
    };
    bool hasGrad() {
        return true;
    }
    bool hasHess() {
        return false;
    }
    s_type operator()(const v_type &);
    v_type getGrad(const v_type &);
    m_type getHess(const v_type &) {
        return m_type();
    };
};
}
#endif // _GEOSFUNC_H

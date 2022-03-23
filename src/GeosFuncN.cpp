#include "GeosFuncN.h"

v_type georis::GeosFuncN::operator()(const v_type &v) {
    for (size_t k = 0; k<_storage->size(); ++k)
        for (auto par : (*_storage)[k] )
            *par->pval = v(k);

    v_type e(_constraints.size());
    for (size_t k = 0; k<_constraints.size(); ++k)
        e(k) = _constraints[k]->error();
    return e;
}
m_type georis::GeosFuncN::getJacob(const v_type &v) {
    for (size_t k = 0; k<_storage->size(); ++k)
        for (auto par : (*_storage)[k] )
            *par->pval = v(k);

    m_type ja = m_type::Zero(_constraints.size(),_storage->size()) ;
    for ( unsigned r = 0;r< _constraints.size();++r )
        for ( unsigned c = 0; c < _storage->size();++c )
            for (auto par : (*_storage)[c] )
                ja(r,c) += _constraints[r]->grad(par);
    return ja;
}

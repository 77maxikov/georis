#include "GeosFuncN.h"

v_type georis::GeosFuncN::operator()(const v_type &v) {
    for (unsigned k = 0; k<_storage->size(); ++k)
        *((*_storage)[k]) = v(k);
    v_type e(_constraints.size());
    for (unsigned k = 0; k<_constraints.size(); ++k)
        e(k) = _constraints[k]->error();
    return e;
}
m_type georis::GeosFuncN::getJacob(const v_type &v) {
    for (unsigned k = 0; k<_storage->size(); ++k)
        *((*_storage)[k]) = v(k);
    m_type ja(_constraints.size(),_storage->size());
    for ( unsigned r = 0;r< _constraints.size();++r )
        for ( unsigned c = 0; c < _storage->size();++c )
            ja(r,c) = _constraints[r]->grad((*_storage)[c]);
    return ja;
}

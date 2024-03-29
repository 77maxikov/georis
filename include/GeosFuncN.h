#ifndef _GEOSFUNCN_H
#define _GEOSFUNCN_H
#include "IOptFunc.h"
#include <vector>
#include "IConstraint.h"
#include <stdexcept>

namespace georis{
class GeosFuncN:public IOptFuncN{
    std::vector<IConstraint*> _constraints;
    std::vector<std::vector<paramProxy *> > *_storage;
public:
    GeosFuncN(std::vector< std::vector<paramProxy *> >*s){if ( s == nullptr ) throw std::invalid_argument("GeosFuncN - bad parameters!");_storage = s;}
    void add(IConstraint *c){if ( c == nullptr ) throw std::invalid_argument("GeosFuncN::add - not a constraint!");_constraints.push_back(c);};
    size_t inDim()const{return _storage->size();};
    size_t outDim()const{return  _constraints.size();}
    bool hasJacob()const {return true;}
	 v_type operator()(const v_type &);
	 m_type getJacob(const v_type &);
};
}


#endif // _GEOSFUNCN_H

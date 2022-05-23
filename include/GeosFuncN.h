#ifndef _GEOSFUNCN_H
#define _GEOSFUNCN_H
#include "IOptFunc.h"
#include <vector>
#include "IConstraint.h"
#include <stdexcept>
#include "mooLog.h"
namespace georis{





class GeosFuncN:public IOptFuncN{
public:
    GeosFuncN(std::vector< std::vector<paramProxy *> >*s){
        if ( s == nullptr )
            throw std::invalid_argument("GeosFuncN - bad parameters!");
        m_storage = s;
    }
    void add(IConstraint *con){
        if ( con == nullptr )
            throw std::invalid_argument("GeosFuncN::add - not a constraint!");
        m_constraints.push_back(con);
    };

    size_t inDim()const{return m_storage->size();};
    size_t outDim()const{return  m_constraints.size();}
    bool hasJacob()const {return true;}
    v_type operator()(const v_type &);
    spam_type getJacob(const v_type &);
private:
    std::vector<IConstraint*> m_constraints;
    std::vector<std::vector<paramProxy *> > *m_storage;
    spam_type m_pdvals;
    Eigen::SparseMatrix<std::vector<DiDelegate> > m_didele;

};
}


#endif // _GEOSFUNCN_H

#ifndef SCALER_H
#define SCALER_H
#include "IOptFunc.h"

class ScaledOptFuncN:public IOptFuncN{
public:
    ScaledOptFuncN(IOptFuncN*scaled,const v_type&scaler):m_scaled(scaled){
        m_scaler.resize(scaler.size(), scaler.size());
        for (size_t k = 0; k < scaler.size(); ++k)
            m_scaler.insert(k,k) = scaler(k);
    }
    virtual size_t inDim()const{return m_scaled->inDim();}
    virtual size_t outDim()const{return m_scaled->outDim();}
    virtual v_type operator()(const v_type &x){return (*m_scaled)(x).array().colwise()*m_scaler.diagonal().array();}
    virtual bool hasJacob() const{return m_scaled->hasJacob();}
    virtual spam_type getJacob(const v_type &x){
        return m_scaler*m_scaled->getJacob(x);
    };
private:
    IOptFuncN* m_scaled;
    spam_type m_scaler;
};

#endif

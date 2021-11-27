#ifndef SCALER_H
#define SCALER_H
#include "IOptFunc.h"

class ScaledOptFuncN:public IOptFuncN{
public:
    ScaledOptFuncN(IOptFuncN*scaled,const v_type&scaler):m_scaled(scaled),m_scaler(scaler){}
    virtual size_t inDim()const{return m_scaled->inDim();}
    virtual size_t outDim()const{return m_scaled->outDim();}
    virtual v_type operator()(const v_type &x){return (*m_scaled)(x).array().colwise()*m_scaler.array();}
    virtual bool hasJacob() const{return m_scaled->hasJacob();}
    virtual m_type getJacob(const v_type &x) { return m_scaled->getJacob(x).array().colwise()*m_scaler.array();}
private:
    IOptFuncN* m_scaled;
    v_type m_scaler;
};

#endif

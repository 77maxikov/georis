#ifndef __IOPTFUNC
#define __IOPTFUNC
#include "common_optim.h"

class IOptFunc{
public:
	virtual size_t inDim()const = 0;
};
//=============================================================
class IOptFunc1_1:public IOptFunc{
public:
	virtual bool hasDeriv() = 0;
	virtual s_type operator()(s_type) = 0;
	virtual s_type getDeriv(s_type) = 0;
	size_t inDim()const{return 1;}
};
//=============================================================
class IOptFunc1_N : public IOptFunc{
public:
	virtual bool hasGrad() = 0;
	virtual bool hasHess() = 0;
	virtual s_type operator()(const v_type &) = 0;
	virtual v_type getGrad(const v_type &) = 0;
	virtual m_type getHess(const v_type &) = 0;
};
//=============================================================
class IOptFuncN : public IOptFunc{
public:
	virtual size_t outDim()const = 0;
	virtual v_type operator()(const v_type &) = 0;
	virtual bool hasJacob() = 0;
	virtual m_type getJacob(const v_type &) = 0;
};
#endif

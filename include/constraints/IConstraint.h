#pragma once
#include <vector>
#include "param.h"
namespace georis{
class IConstraint{
public:
	virtual double error()const = 0;
    virtual double grad(const paramProxy *v)const = 0;
    virtual constexpr std::vector<paramProxy*> cparam()const = 0;
	virtual ~IConstraint(){};
protected:
	static const double epsi;
};
}


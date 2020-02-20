#pragma once

namespace georis{
class IConstraint{
public:
	virtual double error()const = 0;
	virtual double grad(const double *v)const = 0;
	virtual ~IConstraint(){};
protected:
	static const double epsi;
};
}


#pragma once
#include <vector>
#include "param.h"
namespace georis{

class DiDelegate
{
public:
    DiDelegate()
        : object_ptr(nullptr)
        , stub_ptr(nullptr)
    {}

    template <class T, double (T::*TMethod)(void)>
    static DiDelegate from_method(T* object_ptr)
    {
        DiDelegate d;
        d.object_ptr = object_ptr;
        d.stub_ptr = &method_stub<T, TMethod>; // #1
        return d;
    }

    double operator()() const
    {
        if (stub_ptr)
            return (*stub_ptr)(object_ptr);
        return 0.0;
    }
    bool empty()const{return object_ptr == nullptr;}

private:
    typedef double (*stub_type)(void* object_ptr);

    void* object_ptr;
    stub_type stub_ptr;

    template <class T, double (T::*TMethod)()>
    static double method_stub(void* object_ptr)
    {
        T* p = static_cast<T*>(object_ptr);
        return (p->*TMethod)(); // #2
    }
};




class IConstraint{
public:
	virtual double error()const = 0;
    virtual DiDelegate grad(const paramProxy *v) = 0;
    virtual constexpr std::vector<paramProxy*> cparam()const = 0;
	virtual ~IConstraint(){};
protected:
	static const double epsi;
};
}


#ifndef _CONSTRPP2LSYM_H
#define _CONSTRPP2LSYM_H
#include "IConstraint.h"
#include "ConstrL2L.h"

#include "objects.h"


namespace georis{
// Points symmetric to line
class ConstrPP2LSym:public ConstrL2L,public IConstraint{
public:
    ConstrPP2LSym(const point2r&beg,const point2r&end,const line2r&l2):ConstrL2L(beg,end,l2){};
    double error()const;
    double grad(const paramProxy *)const;
    std::vector<paramProxy *> cparam()const { return {l11x,l11y,l12x,l12y,l21x,l21y,l22x,l22y}; };
};
}
#endif // _CONSTRPP2LSYM_H

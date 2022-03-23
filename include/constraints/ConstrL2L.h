#ifndef _CONSTRL2L_H
#define _CONSTRL2L_H
#include "objects.h"
namespace georis{
class ConstrL2L{
public:
    ConstrL2L(const line2r&, const line2r&);
    ConstrL2L(const point2r&,const point2r&, const line2r&);
protected:
    paramProxy *l11x,*l11y,*l12x,*l12y;
    paramProxy *l21x,*l21y,*l22x,*l22y;

};
}
#endif // _CONSTRL2L_H

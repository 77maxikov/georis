#ifndef _OBJ_UTILS_H
#define _OBJ_UTILS_H
#include "objects.h"
#include <iostream>

std::ostream & operator << (std::ostream &ost,const georis::point2r &p);
std::ostream & operator << (std::ostream &ost,const georis::line2r &l);

#endif // _GEOOBJ_UTILS_H

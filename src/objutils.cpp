#include "objutils.h"

std::ostream & operator << (std::ostream &ost,const georis::point2r &p){
	ost << '('<<*p.x << ','<<*p.y<<')';
	return ost;
}
std::ostream & operator << (std::ostream &ost,const georis::line2r &l){
	ost << l.beg <<"->"<< l.end;
	return ost;
}


#ifndef _APUTILS
#define _APUTILS

#include <iostream>


template<class T> std::ostream& operator<<(std::ostream &ost,const ap::template_1d_array<T> &ar){
	for (int r = ar.getlowbound();r<=ar.gethighbound();r++){
		ost<<ar(r)<<std::endl;
	}		
	return ost;
}

template<class T> std::ostream& operator<<(std::ostream &ost,const ap::template_2d_array<T> &ar){
	for (int r = ar.getlowbound(1);r<=ar.gethighbound(1);r++)
		for (int c = ar.getlowbound(2);c<=ar.gethighbound(2);c++)
			ost<<ar(r,c)<<(c==ar.gethighbound(2)?'\n': ' ');			
	return ost;
}
#endif
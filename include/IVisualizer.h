#ifndef _IVISUALISER_H
#define _IVISUALISER_H
#include <vector>
#include <string>
#include <map>
#include "georis.h"
#include "uid.h"
#include "objects.h"

#define MODE_NORMAL        0x0
#define MODE_SELECTED      0x1
#define MODE_FIXED         0x2
#define MODE_HIGHLIGHTED   0x4
#define MODE_CONSTRUCTI    0x8




enum InputMode{
	IM_NONE=0,
	IM_POINT,
	IM_LINE,
	IM_CIRCLE,
        IM_ARC,
	IM_RECT
	};

class IVisualizer{
public:
        virtual void reset() = 0;
        virtual void setController(georis::Controller*) = 0;
	virtual void drawPoint(const double *px,const double *py,unsigned status = MODE_NORMAL)=0;
	virtual void drawLine(const double *px1,const double *py1,const double *px2,const double *py2,unsigned status = MODE_NORMAL)=0;
	virtual void drawCircle(const double *px,const double *py,const double *r,unsigned status = MODE_NORMAL)=0;
        virtual void setSelectedObjs(const std::map<UID,std::string> &) = 0;
        virtual void setSelectedConstraints(const std::map<UID,std::string> &) = 0;
        virtual void setAvailConstraints(const std::vector<georis::ConstraintType> &) = 0;
};


#endif // _IVISUALIZER_H

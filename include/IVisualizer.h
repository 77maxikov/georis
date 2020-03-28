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




namespace georis{
class IVisualizer{
public:
        virtual void reset() = 0;
        virtual void setController(Controller*) = 0;

        virtual void drawObject(ObjectType type, const  std::vector<double> &param,unsigned status = MODE_NORMAL) = 0;
        virtual void setSelectionInfo(const std::vector<std::pair<UID,std::string> > &objsSel,
                                 const std::vector< std::pair<UID,std::string> > &constrsSel,
                                 const std::vector<georis::ConstraintType>& constrsAvail) = 0;
};
}

#endif // _IVISUALIZER_H

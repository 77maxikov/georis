#ifndef _IVISUALISER_H
#define _IVISUALISER_H
#include <vector>
#include <string>
#include <map>
#include "georis.h"
#include "uid.h"
#include "objects.h"

namespace georis{
class IVisualizer{
public:
        virtual void reset() = 0;
        virtual void setController(Controller*) = 0;

        virtual void drawObject(ObjectType type, const  std::vector<double> &param,unsigned status = MODE_NORMAL) = 0;
        virtual void displayConstraint(DCType type, double value,const std::vector<double> &param,unsigned status = MODE_NORMAL)=0;
        virtual void setSelectionInfo(const std::vector<std::pair<UID,std::string> > &objsSel,
                                 const std::vector< std::pair<UID,std::string> > &constrsSel,
                                 const std::vector<georis::ConstraintType>& constrsAvail) = 0;
};
}

#endif // _IVISUALIZER_H

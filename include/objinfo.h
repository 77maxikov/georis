#ifndef OBJINFO_H
#define OBJINFO_H
#include "objects.h"
#include "uid.h"

namespace georis{
struct objInfo{
    static const size_t MAXCHILDS = 3;
    sketchObj *obj;
    std::vector<UID> constrs;
    UID objChilds[MAXCHILDS];
    objInfo():obj(nullptr){for (size_t k = 0;k<MAXCHILDS;++k) objChilds[k] = NOUID;}
    bool hasChild(UID uid)const{
        if ( uid == NOUID ) return false;
        size_t k = 0;
        do{
            if ( objChilds[k] == NOUID ) return false;
            if ( objChilds[k] == uid ) return true;
            ++k;
        }while ( k<MAXCHILDS );
        return false;
    }
};
}
#endif // OBJINFO_H

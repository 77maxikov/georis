#ifndef _IFILEIO_H
#define _IFILEIO_H
#include <vector>

#include "errors.h"
#include "georis.h"
#include "uid.h"

namespace georis{

class IFileWriter{
public:
    virtual RESCODE saveObject(UID uid,ObjectType ot,const std::vector<double> &params,UID parent = NOUID) = 0;
    virtual RESCODE saveConstraint(UID uid,ConstraintType ct, const std::vector<UID>&constrobj,double *param = nullptr) = 0;
};
class IFileReader{
public:
    virtual RESCODE loadObject(UID &uid,ObjectType &ot,std::vector<double> &params, UID &parent) = 0;
    virtual RESCODE loadConstraint(UID &uid,ConstraintType &ct, std::vector<UID>&, double &param) = 0;
};

}

#endif

#ifndef _IFILEIO_H
#define _IFILEIO_H
#include <string>
#include <vector>

#include "errors.h"
#include "georis.h"
#include "uid.h"

namespace georis{

class IFileWriter{
public:
    virtual ~IFileWriter(){}
    virtual RESCODE prepare(const std::string& filename) = 0;
    virtual RESCODE saveObject(
            UID uid,
            const std::string & name,
            ObjectType ot,
            const std::vector<double> &params,
            unsigned attributes,
            UID parent = NOUID) = 0;
    virtual RESCODE saveConstraint(
            UID uid,
            const std::string & name,
            ConstraintType ct,
            const std::vector<UID>&constrobj,
            double *param = nullptr) = 0;
};
class IFileReader{
public:
    virtual RESCODE loadObject(UID &uid,std::string & name, ObjectType &ot,std::vector<double> &param,unsigned &attributes, UID &parent) = 0;
    virtual RESCODE loadConstraint(UID &uid,std::string & name, ConstraintType &ct, std::vector<UID>&, double &param) = 0;
    virtual UID getLastUID() = 0;
};

}

#endif

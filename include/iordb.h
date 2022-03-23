#ifndef RDBWRITER_H
#define RDBWRITER_H

#include "IFileIO.h"
#include <fstream>
#include <stack>

namespace georis{

class RDBWriter: public IFileWriter
{
public:
    RDBWriter(){m_uNumObjs = 0;}
    virtual ~RDBWriter();
    virtual RESCODE prepare(const std::string& fname);
    virtual RESCODE saveObject(UID uid,const std::string &name, ObjectType ot,const std::vector<double> &params,unsigned attributes,UID parent = NOUID);
    virtual RESCODE saveConstraint(UID uid,const std::string &name, ConstraintType ct, const std::vector<UID>&constrobj,double *paramProxy = nullptr);
private:
   unsigned m_uNumObjs;
   const char *m_szFName;
   std::fstream m_fs;
   std::stack<std::pair<unsigned,std::string> > m_sta;
};

class RDBReader: public IFileReader
{
public:
    RESCODE load(const char *fname);
    virtual ~RDBReader();
    virtual RESCODE loadObject(UID &uid,std::string &name, ObjectType &ot,std::vector<double> &paramProxys, UID &parent);
    virtual RESCODE loadConstraint(UID &uid,std::string &name, ConstraintType &ct, std::vector<UID>&, double &paramProxy);
private:
};

}

#endif // RDBWRITER_H

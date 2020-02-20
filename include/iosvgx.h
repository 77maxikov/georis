#ifndef IOSVGX_H
#define IOSVGX_H
#include "IFileIO.h"
#include "tinyxml2.h"

namespace georis{

class SVGXWriter: public IFileWriter
{
public:    
    RESCODE prepare(const char *fname);
    virtual ~SVGXWriter();
    virtual RESCODE saveObject(UID uid,ObjectType ot,const std::vector<double> &params,UID parent = NOUID);
    virtual RESCODE saveConstraint(UID uid,ConstraintType ct, const std::vector<UID>&constrobj,double *param = nullptr);
private:
   tinyxml2::XMLDocument m_Doc;
   const char *m_szFName;
   tinyxml2::XMLElement *m_RootElem;
};

class SVGXReader: public IFileReader
{
public:   
    RESCODE load(const char *fname);
    virtual ~SVGXReader();
    virtual RESCODE loadObject(UID &uid,ObjectType &ot,std::vector<double> &params, UID &parent);
    virtual RESCODE loadConstraint(UID &uid,ConstraintType &ct, std::vector<UID>&, double &param);
private:
   tinyxml2::XMLDocument m_Doc;
   tinyxml2::XMLElement *m_CurrentElem;
};

}

#endif // IOSVGX_H

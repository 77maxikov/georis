#ifndef IOSVGX_H
#define IOSVGX_H

#include "IFileIO.h"
#include "tinyxml2.h"


namespace georis{

class SVGXWriter: public IFileWriter
{
public:    
    virtual ~SVGXWriter();
    virtual RESCODE prepare(const std::string& fname);
    virtual RESCODE saveObject(UID uid,const std::string &name, ObjectType ot,const std::vector<double> &params,unsigned attributes,UID parent = NOUID);
    virtual RESCODE saveConstraint(UID uid,const std::string &name, ConstraintType ct, const std::vector<UID>&constrobj,double *param = nullptr);
private:
   tinyxml2::XMLDocument m_Doc;
   std::string m_sFName;
   tinyxml2::XMLElement *m_RootElem;
};

class SVGXReader: public IFileReader
{
public:   
    RESCODE load(const char *fname);
    virtual ~SVGXReader();
    virtual RESCODE loadObject(UID &uid,std::string &name, ObjectType &ot,std::vector<double> &params,unsigned &attributes, UID &parent);
    virtual RESCODE loadConstraint(UID &uid,std::string &name, ConstraintType &ct, std::vector<UID>&, double &param);
    virtual UID getLastUID();
private:
   tinyxml2::XMLDocument m_Doc;
   tinyxml2::XMLElement *m_CurrentElem;
   UID m_lastUID;
};

}

#endif // IOSVGX_H

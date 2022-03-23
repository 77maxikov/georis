#include <cassert>
#include "iosvgx.h"

#define DOC_WIDTH 1000
#define DOC_HEIGHT 1000

RESCODE georis::SVGXWriter::prepare(const std::string& fname){
    m_sFName = fname.c_str();
    m_RootElem = m_Doc.NewElement("svg");
    if (m_RootElem == nullptr) return RC_RUNTIME_ERR;

    m_RootElem->SetAttribute("width",DOC_WIDTH);
    m_RootElem->SetAttribute("height",DOC_HEIGHT);
    m_Doc.InsertFirstChild(m_RootElem);

    return RC_OK;
}
georis::SVGXWriter::~SVGXWriter(){

    m_Doc.SaveFile(m_sFName.c_str());
    //m_Doc.Clear();
}
RESCODE georis::SVGXWriter::saveObject(
        UID uid,
        const std::string &name,
        ObjectType ot,
        const std::vector<double> &params,
        unsigned attributes,
        UID parent){
    switch (ot){
    case OT_POINT:
    {
        tinyxml2::XMLElement *elem = m_Doc.NewElement("point");
        if (elem == nullptr) return RC_RUNTIME_ERR;
        elem->SetAttribute("id",static_cast<unsigned>(uid));
        assert(params.size() == 2);
        elem->SetAttribute("x",params[0]);
        elem->SetAttribute("y",params[1]);
        elem->SetAttribute("name",name.c_str());
        if (parent != NOUID)
            elem->SetAttribute("parentid",static_cast<unsigned>(parent));
        m_RootElem->InsertEndChild(elem);
        //m_Doc.InsertAfterChild(m_RootElem,elem);
        break;
    }
    case OT_SEGMENT:
    {
        tinyxml2::XMLElement *elem = m_Doc.NewElement("segment");
        if (elem == nullptr) return RC_RUNTIME_ERR;
        elem->SetAttribute("id",(unsigned)uid);
        assert(params.size() == 4);
        elem->SetAttribute("x1",params[0]);
        elem->SetAttribute("y1",params[1]);
        elem->SetAttribute("x2",params[2]);
        elem->SetAttribute("y2",params[3]);
        elem->SetAttribute("name",name.c_str());

        m_RootElem->InsertEndChild(elem);

        break;
    }
    case OT_CIRCLE:
    {
        tinyxml2::XMLElement *elem = m_Doc.NewElement("circle");
        if (elem == nullptr) return RC_RUNTIME_ERR;
        elem->SetAttribute("id",(unsigned)uid);
        assert(params.size() == 3);
        elem->SetAttribute("x",params[0]);
        elem->SetAttribute("y",params[1]);
        elem->SetAttribute("r",params[2]);
        elem->SetAttribute("name",name.c_str());

        m_RootElem->InsertEndChild(elem);
        break;
    }
    case OT_ARC:{
        tinyxml2::XMLElement *elem = m_Doc.NewElement("arc");
        if (elem == nullptr) return RC_RUNTIME_ERR;
        elem->SetAttribute("id",(unsigned)uid);
        assert(params.size() == 6);
        elem->SetAttribute("xc",params[0]);
        elem->SetAttribute("yc",params[1]);
        elem->SetAttribute("xs",params[2]);
        elem->SetAttribute("ys",params[3]);
        elem->SetAttribute("xf",params[4]);
        elem->SetAttribute("yf",params[5]);
        elem->SetAttribute("name",name.c_str());

        m_RootElem->InsertEndChild(elem);

        break;
    }
    default:

        break;
    }

    return RC_OK;
}

RESCODE georis::SVGXWriter::saveConstraint(UID uid, const std::string &name, ConstraintType ct, const std::vector<UID> &constrobj, double *parame){
    tinyxml2::XMLElement *elem = m_Doc.NewElement("constraint");

    if (elem == nullptr) return RC_RUNTIME_ERR;

    elem->SetAttribute("id",static_cast<unsigned>(uid));
    elem->SetAttribute("type", static_cast<unsigned>(ct));
    elem->SetAttribute("name",name.c_str());

    if (parame != nullptr)
        elem->SetAttribute("param", *parame);
    for (auto it: constrobj){
        tinyxml2::XMLElement *elco = m_Doc.NewElement("obj");
        if (elco == nullptr) return RC_RUNTIME_ERR;
        elco->SetAttribute("idref",static_cast<unsigned>(it));
        elem->InsertEndChild(elco);
    }
    m_RootElem->InsertEndChild(elem);
    return RC_OK;
}

RESCODE georis::SVGXReader::load(const char *fname){
    tinyxml2::XMLError eResult = m_Doc.LoadFile(fname);
    if ( eResult != tinyxml2::XML_SUCCESS ) return RC_INVALIDARG;
    m_CurrentElem = m_Doc.FirstChildElement("svg");
    if ( m_CurrentElem == nullptr ) return RC_INVALIDARG;
    m_CurrentElem = m_CurrentElem->FirstChildElement();
    m_lastUID = NOUID;
    return RC_OK;
}
UID georis::SVGXReader::getLastUID(){
    return m_lastUID;
}
georis::SVGXReader::~SVGXReader(){
}

RESCODE georis::SVGXReader::loadObject(UID &uid,std::string &name,ObjectType &ot,std::vector<double> &params,unsigned &attributes, UID &parent){
    if (m_CurrentElem == nullptr) return RC_NO_OBJ;
    if ( tinyxml2::XML_SUCCESS != m_CurrentElem->QueryUnsignedAttribute("id", reinterpret_cast<unsigned *>(&uid) ) )
        return RC_RUNTIME_ERR;
    params.clear();
    char buffer[128];
    if ( strncmp("point",m_CurrentElem->Name(),5) == 0 ){
        ot = OT_POINT;
        params.resize(2);
        if ( tinyxml2::XML_SUCCESS != m_CurrentElem->QueryDoubleAttribute("x", &(params[0])) ) return RC_RUNTIME_ERR;
        if ( tinyxml2::XML_SUCCESS != m_CurrentElem->QueryDoubleAttribute("y", &(params[1])) ) return RC_RUNTIME_ERR;

        const char *szName = nullptr;
        if ( nullptr == (szName = m_CurrentElem->Attribute("name")) ) return RC_RUNTIME_ERR;
        name = szName;

        if ( tinyxml2::XML_SUCCESS != m_CurrentElem->QueryUnsignedAttribute("parentid", reinterpret_cast<unsigned *>(&parent) ) )
             parent = NOUID;

    }
    else if ( strncmp("segment",m_CurrentElem->Name(),4) == 0 ){
        ot = OT_SEGMENT;
        params.resize(4);
        if ( tinyxml2::XML_SUCCESS != m_CurrentElem->QueryDoubleAttribute("x1", &(params[0])) ) return RC_RUNTIME_ERR;
        if ( tinyxml2::XML_SUCCESS != m_CurrentElem->QueryDoubleAttribute("y1", &(params[1])) ) return RC_RUNTIME_ERR;
        if ( tinyxml2::XML_SUCCESS != m_CurrentElem->QueryDoubleAttribute("x2", &(params[2])) ) return RC_RUNTIME_ERR;
        if ( tinyxml2::XML_SUCCESS != m_CurrentElem->QueryDoubleAttribute("y2", &(params[3])) ) return RC_RUNTIME_ERR;

        const char *szName = nullptr;
        if ( nullptr == (szName = m_CurrentElem->Attribute("name")) ) return RC_RUNTIME_ERR;
        name = szName;

        parent = NOUID;
    }
    else if ( strncmp("circle",m_CurrentElem->Name(),6) == 0 ){
        ot = OT_CIRCLE;
        params.resize(3);
        if ( tinyxml2::XML_SUCCESS != m_CurrentElem->QueryDoubleAttribute("x", &(params[0])) ) return RC_RUNTIME_ERR;
        if ( tinyxml2::XML_SUCCESS != m_CurrentElem->QueryDoubleAttribute("y", &(params[1])) ) return RC_RUNTIME_ERR;
        if ( tinyxml2::XML_SUCCESS != m_CurrentElem->QueryDoubleAttribute("r", &(params[2])) ) return RC_RUNTIME_ERR;
        const char *szName = nullptr;
        if ( nullptr == (szName = m_CurrentElem->Attribute("name")) ) return RC_RUNTIME_ERR;
        name = szName;

        parent = NOUID;
    }
    else if ( strncmp("arc",m_CurrentElem->Name(),3) == 0 ){
        ot = OT_ARC;
        params.resize(6);
        if ( tinyxml2::XML_SUCCESS != m_CurrentElem->QueryDoubleAttribute("xc", &(params[0])) ) return RC_RUNTIME_ERR;
        if ( tinyxml2::XML_SUCCESS != m_CurrentElem->QueryDoubleAttribute("yc", &(params[1])) ) return RC_RUNTIME_ERR;
        if ( tinyxml2::XML_SUCCESS != m_CurrentElem->QueryDoubleAttribute("xs", &(params[2])) ) return RC_RUNTIME_ERR;
        if ( tinyxml2::XML_SUCCESS != m_CurrentElem->QueryDoubleAttribute("ys", &(params[3])) ) return RC_RUNTIME_ERR;
        if ( tinyxml2::XML_SUCCESS != m_CurrentElem->QueryDoubleAttribute("xf", &(params[4])) ) return RC_RUNTIME_ERR;
        if ( tinyxml2::XML_SUCCESS != m_CurrentElem->QueryDoubleAttribute("yf", &(params[5])) ) return RC_RUNTIME_ERR;
        const char *szName = nullptr;
        if ( nullptr == (szName = m_CurrentElem->Attribute("name")) ) return RC_RUNTIME_ERR;
        name = szName;
        parent = NOUID;
    }

    else return RC_NO_OBJ;
    m_CurrentElem = m_CurrentElem->NextSiblingElement();
    if ( m_lastUID < uid) m_lastUID = uid;
    return RC_OK;
}
RESCODE georis::SVGXReader::loadConstraint(UID &uid,std::string & name, ConstraintType &ct, std::vector<UID>&uids, double &parame){
    if ( m_CurrentElem == nullptr) return RC_NO_OBJ;
    if ( strncmp("constraint",m_CurrentElem->Name(),10) != 0 ) return RC_NO_OBJ;

    uids.clear();

    if ( tinyxml2::XML_SUCCESS != m_CurrentElem->QueryUnsignedAttribute("id", reinterpret_cast<unsigned *>(&uid) ) )
        return RC_RUNTIME_ERR;

    if ( tinyxml2::XML_SUCCESS != m_CurrentElem->QueryUnsignedAttribute("type", reinterpret_cast<unsigned *>(&ct) ) )
        return RC_RUNTIME_ERR;
    if ( tinyxml2::XML_SUCCESS != m_CurrentElem->QueryDoubleAttribute("param", &parame ) )
        parame = 0.0;

    tinyxml2::XMLElement *elco = m_CurrentElem->FirstChildElement("obj");
    while ( elco != nullptr ){
        UID oui = NOUID;
        if (  tinyxml2::XML_SUCCESS != elco->QueryUnsignedAttribute("idref",reinterpret_cast<unsigned *>(&oui)) )
            return RC_RUNTIME_ERR;
        uids.push_back(oui);

        elco = elco->NextSiblingElement();
    }
    const char *szName = nullptr;
    if ( nullptr == (szName = m_CurrentElem->Attribute("name")) ) return RC_RUNTIME_ERR;
    name = szName;
    m_CurrentElem = m_CurrentElem->NextSiblingElement();
    if ( m_lastUID < uid) m_lastUID = uid;
    return RC_OK;
}


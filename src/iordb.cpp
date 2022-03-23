#include "iordb.h"

#include <set>
#include <cassert>
#include <sstream>
#include "mooLog.h"

RESCODE georis::RDBWriter::prepare(const std::string& fname){
    m_uNumObjs = 0;
    m_szFName = fname.c_str();
    m_fs.open(fname,std::ios_base::out|std::ios_base::trunc);
    if ( !m_fs.is_open() ){
        MOOLOG << "RDBWriter: can't open file " << fname << std::endl;
        return RC_RUNTIME_ERR;
    }

    m_fs << "USE sketching ;" << std::endl;

    m_fs << "INSERT INTO  `objtype` (`idobjtype`,`name`,`freedegree`) VALUES (1,'Segment',4);" << std::endl;
    m_fs << "INSERT INTO  `objtype` (`idobjtype`,`name`,`freedegree`) VALUES (5,'Circle',3);" << std::endl;
    m_fs << "INSERT INTO  `objtype` (`idobjtype`,`name`,`freedegree`) VALUES (6,'Arc',5); " << std::endl;

    m_fs << "INSERT INTO `constrtype` (`idconstrtype`,`name`,`is_parametric`) VALUES (0,'Fixed',0);" << std::endl;
    m_fs << "INSERT INTO `constrtype` (`idconstrtype`,`name`,`is_parametric`) VALUES (1,'Equal',0);" << std::endl;
    m_fs << "INSERT INTO `constrtype` (`idconstrtype`,`name`,`is_parametric`) VALUES (2,'Vertical',0);" << std::endl;
    m_fs << "INSERT INTO `constrtype` (`idconstrtype`,`name`,`is_parametric`) VALUES (3,'Horizontal',0);" << std::endl;
    m_fs << "INSERT INTO `constrtype` (`idconstrtype`,`name`,`is_parametric`) VALUES (4,'Distance',1);" << std::endl;
    m_fs << "INSERT INTO `constrtype` (`idconstrtype`,`name`,`is_parametric`) VALUES (5,'Angle',1);" << std::endl;
    m_fs << "INSERT INTO `constrtype` (`idconstrtype`,`name`,`is_parametric`) VALUES (6,'Parallel',0);" << std::endl;
    m_fs << "INSERT INTO `constrtype` (`idconstrtype`,`name`,`is_parametric`) VALUES (7,'Ortho',0);" << std::endl;
    m_fs << "INSERT INTO `constrtype` (`idconstrtype`,`name`,`is_parametric`) VALUES (8,'Tangent',0);" << std::endl;
    m_fs << "INSERT INTO `constrtype` (`idconstrtype`,`name`,`is_parametric`) VALUES (9,'Coincident',0);" << std::endl;
    m_fs << "INSERT INTO `constrtype` (`idconstrtype`,`name`,`is_parametric`) VALUES (10,'Midpoint',0);" << std::endl;
    m_fs << "INSERT INTO `constrtype` (`idconstrtype`,`name`,`is_parametric`) VALUES (11,'Collinear',0);" << std::endl;
    m_fs << "INSERT INTO `constrtype` (`idconstrtype`,`name`,`is_parametric`) VALUES (12,'Dimension',1);" << std::endl;
    m_fs << "INSERT INTO `constrtype` (`idconstrtype`,`name`,`is_parametric`) VALUES (13,'Symmetric',0);" << std::endl;
    m_fs << "INSERT INTO `constrtype` (`idconstrtype`,`name`,`is_parametric`) VALUES (14,'Concentric',0);" << std::endl;
    m_fs << "INSERT INTO `constrtype` (`idconstrtype`,`name`,`is_parametric`) VALUES (15,'Arcus',0);" << std::endl;





    return RC_OK;
}
georis::RDBWriter::~RDBWriter(){

    m_fs.close();

}

int layernum = 1;

RESCODE georis::RDBWriter::saveObject(UID uid,const std::string &name, ObjectType ot,const std::vector<double> &params,unsigned attributes,UID parent){

    MOOLOG << m_sta.size() << std::endl;



    if ( m_uNumObjs % 20 == 0 ){
        ++layernum;
        m_fs << "INSERT INTO `layer` (`idlayer`) VALUES (" << layernum << ");" << std::endl;
    }
    ++m_uNumObjs;

    m_fs << "INSERT INTO `entity` (`identity`,`layer`) VALUES (" << static_cast<unsigned>(uid) << ','<< layernum<< ");" << std::endl;

    switch (ot){
    case OT_POINT:
    {
        assert(params.size() == 2);
        m_fs << "INSERT INTO `point` (`idpoint`,`x`,`y`) VALUES (" << static_cast<unsigned>(uid) << ',' << params[0] << ',' << params[1] << ");" << std::endl;
        if (parent != NOUID){
            std::stringstream ost;
            ost << "INSERT INTO `objpoints` (`idpoint`,`idobject`) VALUES (" << static_cast<unsigned>(uid) << ',' << static_cast<unsigned>(parent) << ");" << std::endl;
            std::pair<unsigned, std::string>  pa = {static_cast<unsigned>(parent),ost.str()};
            m_sta.push(pa);
        }
        break;

    }
    case OT_SEGMENT:
    {
        m_fs << "INSERT INTO `object` (`idobject`, `objtype`)  VALUES (" << static_cast<unsigned>(uid) << ",1);" << std::endl;
        while ( !m_sta.empty() && m_sta.top().first == static_cast<unsigned>(uid) ){
            m_fs << m_sta.top().second;
            m_sta.pop();
        }
        break;
    }
    case OT_CIRCLE:
    {
        m_fs << "INSERT INTO `object` (`idobject`, `objtype`)  VALUES (" << static_cast<unsigned>(uid) << ",5);" << std::endl;
        while ( !m_sta.empty() && m_sta.top().first == static_cast<unsigned>(uid) ){
            m_fs << m_sta.top().second;
            m_sta.pop();
        }
        break;
    }
    case OT_ARC:{
        m_fs << "INSERT INTO `object` (`idobject`, `objtype`)  VALUES (" << static_cast<unsigned>(uid) << ",6);" << std::endl;
        while ( !m_sta.empty() && m_sta.top().first == static_cast<unsigned>(uid) ){
            m_fs << m_sta.top().second;
            m_sta.pop();
        }
        break;
    }
    default:

        break;
    }

    return RC_OK;
}

RESCODE georis::RDBWriter::saveConstraint(UID uid, const std::string &name, ConstraintType ct, const std::vector<UID> &constrobj, double *parame){

    m_fs << "INSERT INTO `entity` (`identity`,`layer`) VALUES (" << static_cast<unsigned>(uid) << ','<< layernum<< ");" << std::endl;

    if (parame != nullptr){
        m_fs << "INSERT INTO `constraint` (`idconstraint`,`constrtype`,`parameter`) VALUES (" << static_cast<unsigned>(uid) << ','<< static_cast<unsigned>(ct) << ',' << *parame << ");" << std::endl;
    }
    else{
        m_fs << "INSERT INTO `constraint` (`idconstraint`,`constrtype`) VALUES (" << static_cast<unsigned>(uid) << ','<< static_cast<unsigned>(ct) << ");" << std::endl;
    }
    std::set<int> sorte;

    for (auto it: constrobj){
        sorte.insert(it);
    }
    for (auto it: sorte){
        m_fs << "INSERT INTO `constrinfo` (`idconstraint`,`identity`) VALUES(" << static_cast<unsigned>(uid) << ',' << static_cast<unsigned>(it) << ");" << std::endl;
    }
    return RC_OK;
}

#ifndef GEOS_CORE_H
#define GEOS_CORE_H
#include <list>
#include <map>
#include <vector>
#include <set>
#include <cmath>

#include "georis.h"
#include "errors.h"
#include "uid.h"
#include "objects.h"
#include "objinfo.h"

#include "IConstraint.h"

namespace georis{
class Core {
public:
    Core();    
    RESCODE createSketch(UID *puid = nullptr);
    RESCODE setActiveSketch(UID);
    RESCODE removeSketch(UID);

    RESCODE addObject(ObjectType type, const std::vector<double>&param,UID *puid = nullptr,std::vector<UID> *pchuids = nullptr);
    RESCODE removeObject(UID);

    // enumerate objects on active sketch
    void enumObjs(std::vector<UID>&)const;

    RESCODE getObjType(UID,ObjectType &ot)const;
    RESCODE getObjParam(UID, std::vector<double>&param) const;
    RESCODE queryObjInfo(UID,ObjectType &ot,std::vector<double>&param)const;
    RESCODE setObjParam(UID, const std::vector<double>&param);      
    RESCODE getObjParent(UID uid,UID& )const;
    RESCODE getObjChilds(UID uid,std::vector<UID>&)const;
    RESCODE getObjConstraints(UID uid,std::vector<UID>&)const;

    RESCODE addConstraint(ConstraintType type,const std::vector<UID> &objs,double param = 0,UID *puid = nullptr);
    RESCODE removeConstraint(UID);

    void enumConstraints(std::vector<UID>&)const;

    RESCODE queryConstrInfo(UID,ConstraintType &type,std::vector<UID> &objs,double *pparam = nullptr) const;
    RESCODE setConstrParam(UID, double param);

    void findObjInCirc(const point2r&,double radius,std::vector<UID> &objs,std::vector<double> *pdists = nullptr)const;
    void findObjInRect(const point2r &tl,const point2r&br,std::vector<UID> &)const;

    RESCODE moveObjects(const std::vector<UID>& objs,double dx, double dy);    

    void filterChildObj(std::vector<UID> &objs)const;

private:
    UID internalAddPoint(double x,double y,ptrep** res = nullptr);
    void internalRemovePoint(UID);



    int backupState();
    int restoreState();
    int tryAddConstraint(ConstraintType type,const std::vector<UID> &objs,double param,UID *puid);
    void groupObj(const std::vector<UID> &uids, std::map<ObjectType,std::vector<objInfo*> > &grouped);
	int solve();



    std::list<double> _params;
    std::vector<double> _paramsBU;
    std::set<double*> _const_params;
    std::map<double*,double*> _linked_params; // ????

    std::map<UID, objInfo> _objects;

	struct ceFunc{
		IConstraint* constr;// Constraint error func
		std::vector<double*> cparam; // parameters linked with this constraint
	};
	struct constrInfo{
		ConstraintType type;
		std::vector<ceFunc> constrs;
		std::vector<UID> objs; // UIDs of constrained objects
	};
    std::map<UID, constrInfo> _constraints;
    std::map<UID, constrInfo> _constraintsBU;

    std::map<UID,int> m_Sketches;
};
}
#endif // GEOSCORE_H

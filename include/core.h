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

    RESCODE addObject(ObjectType type, const std::vector<double>&parame,UID *puid = nullptr,std::vector<UID> *pchuids = nullptr);
    RESCODE removeObject(UID);

    // enumerate objects on active sketch
    void enumObjs(std::vector<UID>&)const;

    RESCODE getObjType(UID,ObjectType &ot)const;
    RESCODE getObjParam(UID, std::vector<double>&param) const;
    RESCODE queryObjInfo(UID,ObjectType &ot,std::vector<double>&param, size_t& freedeg)const;
    RESCODE setObjParam(UID, const std::vector<double>&param);
    RESCODE getObjParent(UID uid,UID& )const;
    RESCODE getObjChilds(UID uid,std::vector<UID>&)const;
    RESCODE getObjConstraints(UID uid,std::vector<UID>&)const;

    RESCODE addConstraint(ConstraintType type,const std::vector<UID> &objs,double param = 0,UID *puid = nullptr);
    RESCODE removeConstraint(UID);

    void enumConstraints(std::vector<UID>&)const;

    RESCODE queryConstrInfo(UID,ConstraintType &type,std::vector<UID> &objs,double *pparam = nullptr) const;
    RESCODE setConstrParam(UID, double parame);

    double findNearest(double x,double y,std::vector<UID> &objs)const;
    double findObjInCirc(double x,double y,double radius,std::vector<UID> &objs,std::vector<double> *pdists = nullptr)const;
    void findObjInRect(double minx,double miny, double maxx,double maxy, std::vector<UID> &)const;

    RESCODE moveObjects(const std::vector<UID>& objs,double dx, double dy);    

    void filterChildObj(std::vector<UID> &objs)const;
    AABBr calcAABB()const;
private:
    UID internalAddPoint(double x,double y,ptrep** res = nullptr);
    void internalRemovePoint(UID);



    int backupState();
    int restoreState();

    void groupObjUIDsByType(const std::vector<UID> &uids, std::map<ObjectType,std::vector<UID> > &grouped)const;


    RESCODE tryAddConstraint(ConstraintType type,const std::vector<UID> &objs,double paramProxy,UID *puid);
    int solve();

    int findConstrGroupByConstrID(UID)const;
    int findConstrGroupByObjID(UID)const;
    int mergeConstrGroups(std::vector<int>&inds);
    void splitConstrGroup(size_t ind);
    void updateConstrGroups2Obj(UID, UID);
    void removeFixedParameters(std::vector<paramProxy*>& paramProxy );


    std::list<double> _paramVals;
    std::list<paramProxy> _params;
    std::vector<double> _paramsBU;
    std::set<double*> _const_params;

    std::map<UID, objInfo> m_objects;

	struct constrInfo{
        constrInfo(){}
        constrInfo(ConstraintType ct,
                   const std::vector<ErrorType>& erty,
                   const std::vector<IConstraint*>& er,
                   const std::vector<UID>& ob,paramProxy* pp = nullptr):type(ct),errTypes(erty),errors(er),objs(ob),param(pp){};
        ConstraintType type;
        std::vector<ErrorType> errTypes; // Types of error functions implementing the constraint
        std::vector<IConstraint*> errors; // Error functions for constraint
		std::vector<UID> objs; // UIDs of constrained objects
        paramProxy* param; // Constraint param, if applicable
	};
    struct constrGroup{
        std::map<UID, constrInfo> constraints;
        bool unsolved;
        std::set<paramProxy*> constants;

        constrGroup():unsolved(true){}

        std::vector<std::set<paramProxy*> > groupEqParams()const;
        void operator+=(const constrGroup&);
        void linkEqualParams(std::vector<std::set<paramProxy*> >&);
        void unlinkEqualParams(std::vector<std::set<paramProxy*> >&);
        void updateEqualParamOrigVals(std::vector<std::set<paramProxy*> >&);

        bool verifyTransitive(ConstraintType ct, const std::vector<UID> &elements);
    };

    std::vector<constrGroup> m_constrGroups;
    std::vector<constrGroup> m_constrGroupsBU;


    std::map<UID,int> m_Sketches;

    void constrainEntities(const std::vector<UID>& objuids, constrInfo& cinfo, UID* puid);
};
}
#endif // GEOSCORE_H

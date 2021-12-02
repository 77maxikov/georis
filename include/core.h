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

    void groupObjUIDsByType(const std::vector<UID> &uids, std::map<ObjectType,std::vector<UID> > &grouped)const;


    RESCODE tryAddConstraint(ConstraintType type,const std::vector<UID> &objs,double param,UID *puid);
    int solve();

    int findConstrGroupByConstrID(UID)const;
    int findConstrGroupByObjID(UID)const;
    void mergeConstrGroups(int,int);
    void updateConstrGroups2Obj(UID, UID);
    void removeFixedParameters(std::vector<double*>& param );


    std::list<double> _params;
    std::vector<double> _paramsBU;
    std::set<double*> _const_params;

    std::map<UID, objInfo> m_objects;

	struct constrInfo{
        constrInfo(){}
        constrInfo(ConstraintType ct,
                   const std::vector<IConstraint*>& er,
                   const std::vector<UID>& ob):type(ct),errors(er),objs(ob){};
        ConstraintType type;
        std::vector<IConstraint*> errors; // Error functions for constraint
		std::vector<UID> objs; // UIDs of constrained objects
	};
    struct constrGroup{
        std::map<UID, constrInfo> constraints;
        bool unsolved;

        constrGroup():unsolved(true){}
        std::vector<double*> getAllParams()const;
        std::vector<double*> getTunableParams()const;

        bool isLinkingConstr(UID) const;


        bool verifyTransitive(ConstraintType ct, const std::vector<UID> &elements);
    };

    std::vector<constrGroup> m_constrGroups;
    std::vector<constrGroup> m_constrGroupsBU;


    std::map<UID,int> m_Sketches;

    void constrainEntities(const std::vector<UID>& objuids, constrInfo& cinfo, UID* puid);

};
}
#endif // GEOSCORE_H

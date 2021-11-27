#include <limits>
#include <stack>
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

#include "core.h"
#include "constraints.h"

#include "constraints.h"
#include "SolverLM.h"
#include "SolverNG.h"
#include "SolverCeres.h"
#include "GeosFunc.h"
#include "GeosFuncN.h"

#include "mooLog.h"
#include "objutils.h"

georis::Core::Core() {
    //ctor
}

RESCODE georis::Core::createSketch(UID *puid){
    *puid = UIDGen::instance()->generate();
    m_Sketches[*puid] = 0;
    return RC_OK;
}
RESCODE georis::Core::setActiveSketch(UID){
    return RC_OK;
}
RESCODE georis::Core::removeSketch(UID){
    return RC_OK;
}

RESCODE georis::Core::addObject(georis::ObjectType type, const std::vector<double>&param,UID *puid,std::vector<UID> *pchuids) {
    // validate uid if given
    if ( puid != nullptr && *puid != NOUID && m_objects.find(*puid) != m_objects.end() ) return RC_NOTUNIQUE;

    switch (type) {
    case OT_POINT: {
        if (param.size() != 2 ) return RC_INVALIDARG;
        if (puid != nullptr){
            if ( *puid == NOUID )   *puid = internalAddPoint(param[0],param[1]);
            else{
                _params.push_back(param[0]);
                double *px = &_params.back();
                _params.push_back(param[1]);
                double *py = &_params.back();
                objInfo info;
                ptrep * pt = new ptrep(px,py);
                info.obj = pt;
                m_objects[*puid] = info;
            }
        }
        else
            internalAddPoint(param[0],param[1]);
        break;
    }
    case OT_SEGMENT: {
        if ( puid != nullptr && *puid != NOUID ) {
            if ( pchuids == nullptr || pchuids->size() != 2) return RC_INVALIDARG;
            // validate if children are points
            auto ptit0 = m_objects.find((*pchuids)[0]);
            if ( ptit0 == m_objects.end() ) return RC_NO_OBJ;
            if ( (*ptit0).second.obj->type() != OT_POINT ) return RC_INVALIDARG;
            auto ptit1 = m_objects.find((*pchuids)[1]);
            if ( ptit1 == m_objects.end() ) return RC_NO_OBJ;
            if ( (*ptit1).second.obj->type() != OT_POINT ) return RC_INVALIDARG;

            objInfo info;
            info.obj = new lirep( dynamic_cast<ptrep*>( (*ptit0).second.obj ), dynamic_cast<ptrep*>( (*ptit1).second.obj ));
            info.objChilds[0] = (*pchuids)[0];
            info.objChilds[1] = (*pchuids)[1];
            m_objects[*puid] = info;
        }
        else {
            if (param.size() != 4 ) return RC_INVALIDARG;
            ptrep* pt1 = nullptr;
            UID uidp1 = internalAddPoint(param[0],param[1],&pt1);
            ptrep* pt2 = nullptr;
            UID uidp2 = internalAddPoint(param[2],param[3],&pt2);
            UID uid = UIDGen::instance()->generate();
            objInfo info;
            info.obj = new lirep(pt1,pt2);
            info.objChilds[0] = uidp1;
            info.objChilds[1] = uidp2;
            m_objects[uid] = info;

            if ( puid != nullptr ) *puid = uid;
        }
        break;
    }
    case OT_CIRCLE: {
        if ( puid != nullptr && *puid != NOUID ) {
            if ( pchuids == nullptr || pchuids->size() != 1 ) return RC_INVALIDARG;

            // validate if children are points
            auto ptit0 = m_objects.find((*pchuids)[0]);
            if ( ptit0 == m_objects.end() ) return RC_NO_OBJ;
            if ( (*ptit0).second.obj->type() != OT_POINT ) return RC_INVALIDARG;

            objInfo info;
            _params.push_back(param[2]);
            info.obj = new circrep( dynamic_cast<ptrep*>( (*ptit0).second.obj ),&_params.back());
            info.objChilds[0] = (*pchuids)[0];
            m_objects[*puid] = info;
        }
        else{
            ptrep* pt = nullptr;
            UID uidp = internalAddPoint(param[0],param[1],&pt);
            _params.push_back(param[2]);
            UID uid = UIDGen::instance()->generate();
            objInfo info;
            info.obj = new circrep(pt,&_params.back());
            info.objChilds[0] = uidp;
            m_objects[uid] = info;
            if ( puid != nullptr ) *puid = uid;
        }
        break;
    }
    case OT_ARC:{
        if ( puid != nullptr && *puid != NOUID ) {
            if ( pchuids == nullptr || pchuids->size() != 3 ) return RC_INVALIDARG;
            // validate if children are points
            auto ptitc = m_objects.find((*pchuids)[0]);
            if ( ptitc == m_objects.end() ) return RC_NO_OBJ;
            if ( (*ptitc).second.obj->type() != OT_POINT ) return RC_INVALIDARG;
            auto ptits = m_objects.find((*pchuids)[1]);
            if ( ptits == m_objects.end() ) return RC_NO_OBJ;
            if ( (*ptits).second.obj->type() != OT_POINT ) return RC_INVALIDARG;
            auto ptitf = m_objects.find((*pchuids)[2]);
            if ( ptitf == m_objects.end() ) return RC_NO_OBJ;
            if ( (*ptitf).second.obj->type() != OT_POINT ) return RC_INVALIDARG;

            objInfo info;
            info.obj = new arcrep(dynamic_cast<ptrep*>( (*ptitc).second.obj ),
                                  dynamic_cast<ptrep*>( (*ptits).second.obj ),
                                  dynamic_cast<ptrep*>( (*ptitf).second.obj ));
            info.objChilds[0] = (*pchuids)[0];
            info.objChilds[1] = (*pchuids)[1];
            info.objChilds[2] = (*pchuids)[2];

            m_objects[*puid] = info;
        }
        else{
            ptrep* ptc = nullptr;
            UID uidc = internalAddPoint(param[0],param[1],&ptc);
            ptrep* pts = nullptr;
            UID uidp1 = internalAddPoint(param[2],param[3],&pts);

            double dx = param[2] - param[0];
            double dy = param[3] - param[1];
            double r = std::sqrt(dx*dx + dy*dy);

            dx = param[4] - param[0];
            dy = param[5] - param[1];
            double re = std::sqrt(dx*dx+dy*dy);

            dx *= r/re;
            dy *= r/re;
            dx += param[0];
            dy += param[1];

            ptrep* pte = nullptr;
            UID uidp2 = internalAddPoint(dx,dy,&pte);

            UID uid = UIDGen::instance()->generate();
            objInfo info;
            info.obj = new arcrep(ptc,pts,pte);
            info.objChilds[0] = uidc;
            info.objChilds[1] = uidp1;
            info.objChilds[2] = uidp2;

            m_objects[uid] = info;
            if (puid) *puid = uid;

            RESCODE res = addConstraint(CT_EQUAL,{uidc,uidp1,uidc,uidp2});
        }
        break;
    }
    case OT_SPLINE:
    case OT_NONE:
    default:
        ;

    }
    return RC_OK;
}

UID georis::Core::internalAddPoint(double x,double y,ptrep** res) {
    _params.push_back(x);
    double *px = &_params.back();
    _params.push_back(y);
    double *py = &_params.back();
    UID uid = UIDGen::instance()->generate();
    objInfo info;
    ptrep * pt = new ptrep(px,py);
    info.obj = pt;
    m_objects[uid] = info;
    if ( res ) *res = pt;
    return uid;
}

void georis::Core::internalRemovePoint(UID pui) {
    auto objit = m_objects.find(pui);
    if ( objit == m_objects.end()) return;

    // Remove constraints
    while ( ! (*objit).second.constrs.empty() )
        removeConstraint( (*objit).second.constrs.front() );

    ptrep* p = dynamic_cast<ptrep*>((*objit).second.obj);
    //Remove params
    auto par = _params.begin();
    while ( par != _params.end() ){
        if ( &(*par) == p->x || &(*par) == p->y ) {
            auto pp = par;
            ++par;
            _params.erase(pp);
            continue;
        }
        ++par;
    }
    delete (*objit).second.obj;
    // Remove point
    m_objects.erase(objit);
}

RESCODE georis::Core::removeObject(UID remUID) {
    MOOLOG << "Core::removeObject: before - objects " << m_objects.size() << " parameters " << _params.size() << std::endl;
    auto objit = m_objects.find(remUID);
    if ( objit == m_objects.end() ) return RC_NO_OBJ;
    sketchObj* obj = (*objit).second.obj;
    switch( obj->type() ) {
    case OT_CIRCLE: {
        circrep* ci = dynamic_cast<circrep*>(obj);
        // Remove constraints
        for (auto it = (*objit).second.constrs.begin();it != (*objit).second.constrs.end();++it)
            removeConstraint(*it);

        for (auto par = _params.begin(); par != _params.end(); ++par ) {
            if ( &(*par) == ci->r ) {
                _params.erase(par);
                break;
            }
        }
        // Remove child objects
        internalRemovePoint( (*objit).second.objChilds[0] );
        // Remove object
        m_objects.erase(objit);
        break;
    }
    case OT_SEGMENT: {
        // Remove constraints
        for (auto it = (*objit).second.constrs.begin();it != (*objit).second.constrs.end();++it)
            removeConstraint(*it);
        // Remove child objects
        internalRemovePoint( (*objit).second.objChilds[0] );
        internalRemovePoint( (*objit).second.objChilds[1] );
        // Remove object
        m_objects.erase(objit);
        break;
    }
    case OT_POINT: {
        // Check if point is not standalone
        for (auto cit = m_objects.begin(); cit != m_objects.end(); ++cit)
            if ( (*cit).second.hasChild(remUID) ) return -1;
        internalRemovePoint(remUID);
        break;
    }
    case OT_ARC:{
        // Remove constraints
        for (auto it = (*objit).second.constrs.begin();it != (*objit).second.constrs.end();++it)
            removeConstraint(*it);
        // Remove child objects
        internalRemovePoint( (*objit).second.objChilds[0] );
        internalRemovePoint( (*objit).second.objChilds[1] );
        internalRemovePoint( (*objit).second.objChilds[2] );
        // Remove object
        m_objects.erase(objit);
        break;
    }
    case OT_SPLINE:
    case OT_LINDIM:
    case OT_CIRCDIM:
    case OT_NONE:
    default:
        ;
    }
    MOOLOG << "Core::removeObject: after - objects " << m_objects.size() << " parameters " << _params.size() << std::endl;
    return RC_OK;
}

RESCODE georis::Core::getObjType(UID uid,ObjectType &ot)const {
    std::vector<double> param;
    return queryObjInfo(uid,ot,param);
}

RESCODE georis::Core::getObjParam(UID uid,std::vector<double>&param)const {
    ObjectType ot;
    return queryObjInfo(uid,ot,param);
}

RESCODE georis::Core::queryObjInfo(UID uid,ObjectType &ot,std::vector<double>&param)const {
    auto objit = m_objects.find(uid);
    if ( objit == m_objects.end() ) return RC_NO_OBJ;

    ot = (*objit).second.obj->type();
    //info.status = (*objit).second.obj->fixed;
    param.clear();
    switch ( ot ){
    case OT_POINT:
    {
        param.reserve(2);
        ptrep *pt = dynamic_cast<ptrep*>((*objit).second.obj);
        param.push_back(*pt->x);
        param.push_back(*pt->y);
        break;
    }
    case OT_SEGMENT:
    {
        param.reserve(4);
        lirep *li = dynamic_cast<lirep*>((*objit).second.obj);
        param.push_back(*li->beg->x);
        param.push_back(*li->beg->y);
        param.push_back(*li->end->x);
        param.push_back(*li->end->y);
        break;
    }
    case OT_CIRCLE:
    {
        param.reserve(3);
        circrep *ci = dynamic_cast<circrep*>((*objit).second.obj);
        param.push_back(*ci->center->x);
        param.push_back(*ci->center->y);
        param.push_back(*ci->r);
        break;
    }
    case OT_ARC:{
        param.reserve(5);
        arcrep *arc = dynamic_cast<arcrep*>((*objit).second.obj);
        param.push_back(*arc->center->x);
        param.push_back(*arc->center->y);
        param.push_back(*arc->beg->x);
        param.push_back(*arc->beg->y);
        param.push_back(*arc->end->x);
        param.push_back(*arc->end->y);

        break;
    }
    case OT_SPLINE:
    case OT_NONE:
    default:
        ;

    }
    return RC_OK;
}
void georis::Core::enumObjs(std::vector<UID> &uids)const{
    uids.clear();
    uids.reserve(m_objects.size());
    for (auto it : m_objects){
        uids.push_back(it.first);
    }
}
RESCODE georis::Core::getObjChilds(UID uid,std::vector<UID>&uids)const {
    uids.clear();
    auto objit = m_objects.find(uid);
    if ( objit == m_objects.end() )
        return RC_NO_OBJ;

    const size_t *start = (*objit).second.objChilds;
    switch ( (*objit).second.obj->type() ){
    case OT_SEGMENT:
        uids = std::vector<UID>(start,start+2);
        break;
    case OT_CIRCLE:
        uids = std::vector<UID>(start,start+1);
        break;
    case OT_ARC:
        uids = std::vector<UID>(start,start+3);
        break;
    case OT_SPLINE:
    case OT_POINT:
    case OT_NONE:
    default:
        ;
    }
    return RC_OK;
}
RESCODE georis::Core::getObjParent(UID uid,UID &par)const{
    for ( auto obj : m_objects)
        if ( obj.second.hasChild(uid) ) { par = obj.first; return RC_OK;}
    par = NOUID;
    return RC_NO_OBJ;
}
RESCODE georis::Core::getObjConstraints(UID uid,std::vector<UID>& res)const {
    res.clear();
    auto objit = m_objects.find(uid);
    if ( objit == m_objects.end() ) return RC_NO_OBJ;
    res = (*objit).second.constrs;
    return RC_OK;
}
void georis::Core::findObjInCirc(const point2r&p,double radius,std::vector<UID> &uids,std::vector<double> *dists)const{
    uids.clear();
    for (auto it = m_objects.begin();it != m_objects.end();++it){
        double dist = (*it).second.obj->dist2point(*(p.x),*(p.y));
        //MOOLOG << "Core::findObjInCirc:  id " << (*it).first << " dist " << dist << std::endl;

        if ( dist < radius ){
            uids.push_back((*it).first);
            if ( dists != nullptr )
                dists->push_back(dist);
        }
    }
}
void georis::Core::findObjInRect(const point2r &pLo,const point2r &pHi,std::vector<UID> &uids)const{
    uids.clear();
    for (auto it = m_objects.begin();it != m_objects.end();++it){
        if ( (*it).second.obj->inRect(*pLo.x,*pLo.y,*pHi.x,*pHi.y) )
            uids.push_back((*it).first);
    }
}

int georis::Core::backupState(){
    _paramsBU.resize(_params.size());
    std::copy(_params.begin(),_params.end(),_paramsBU.begin());
    m_constrGroupsBU = m_constrGroups;
    return 0;
}
int georis::Core::restoreState(){
    if (_params.size() != _paramsBU.size()) return -1;
    std::copy(_paramsBU.begin(),_paramsBU.end(),_params.begin());
    m_constrGroups = m_constrGroupsBU;
    return 0;
}

/*!
Попытка добавить ограничение
\param[in] type Тип ограничения
\param[in] uids Объекты, для которых применяется ограничение
\param[in] param Дополнительный параметр для ограничения
\param[in|out] puid id ограничения
\return результат попытки
*/
RESCODE georis::Core::tryAddConstraint(ConstraintType type,const std::vector<UID> &uids,double param,UID *puid) {
    if ( (puid != nullptr && *puid != NOUID && findConstrGroupByConstrID(*puid) >= 0) || uids.empty() ){
        return RC_INVALIDARG;
    }
    std::map<ObjectType,std::vector<UID> > grouped;
    groupObjUIDsByType(uids,grouped);


    bool added = false;

    switch (type) {
    case CT_FIX:{
        UID construid = NOUID;

        for ( auto objuid : uids ){
            auto objiter = m_objects.find(objuid);
            if ( objiter == m_objects.end() ) return RC_INVALIDARG;

            // find constrgroup of current object
            UID uidpar = NOUID;
            getObjParent(objuid,uidpar);

            int ng = 0;
            if ( uidpar != NOUID ) ng = findConstrGroupByObjID(uidpar);
            else ng = findConstrGroupByObjID(objuid);
            if ( ng < 0 ){
                // Add new group
                if ( puid != nullptr ){
                    if (*puid != NOUID )
                        construid = *puid;
                    else
                        *puid = construid = UIDGen::instance()->generate();
                }
                else
                    construid = UIDGen::instance()->generate();
                constrInfo cinfo(type,{},{objuid} );// CT_FIX

                //add all children of current obj
                size_t c = 0;
                while( c < objInfo::MAXCHILDS && (*objiter).second.objChilds[c] != NOUID ){
                    cinfo.objs.push_back((*objiter).second.objChilds[c]);
                    // fix subObj
                    m_objects[(*objiter).second.objChilds[c++]].obj->fixed = true;
                }
                // fix
                (*objiter).second.constrs.push_back(construid);
                (*objiter).second.obj->fixed = true;
                added = true;

                m_constrGroups.push_back(constrGroup());
                m_constrGroups.back().constraints[construid] = cinfo;
                m_constrGroups.back().unsolved = false;
            }else{
                // if constrgroup found, check if there is a FIXED constraint in it
                bool fixedfound = false;
                for (auto && constr: m_constrGroups[ng].constraints ){
                    if ( constr.second.type == CT_FIX ){
                        fixedfound = true;
                        if ( puid != nullptr ){
                            if ( *puid != NOUID ){
                                if ( *puid != constr.first ) return RC_INVALIDARG;
                            }
                            else
                                *puid = constr.first;
                        }
                        // check if there is an obj in this group with such an id
                        bool objuidfound = false;
                        for ( auto &constrobjid : constr.second.objs )
                            if ( objuid == constrobjid ){
                                objuidfound = true;break;
                            }
                        if ( !objuidfound ){
                            constr.second.objs.push_back(objuid);
                            //add all children of current obj
                            size_t c = 0;
                            while( c < objInfo::MAXCHILDS && (*objiter).second.objChilds[c] != NOUID ){
                                constr.second.objs.push_back((*objiter).second.objChilds[c]);
                                // fix subObj
                                m_objects[(*objiter).second.objChilds[c++]].obj->fixed = true;
                            }
                            // fix
                            (*objiter).second.constrs.push_back(constr.first);
                            (*objiter).second.obj->fixed = true;
                            added = true;
                        }
                        break;
                    }
                }
                if ( !fixedfound ){
                    // add fixed constraint to this group
                    if ( puid != nullptr ){
                        if (*puid != NOUID )
                            construid = *puid;
                        else
                            *puid = construid = UIDGen::instance()->generate();
                    }
                    else
                        construid = UIDGen::instance()->generate();
                    m_constrGroups[ng].constraints[construid].type = type;
                    m_constrGroups[ng].constraints[construid].objs.push_back(objuid);
                    // Add child objects
                    //add all children of current obj
                    size_t c = 0;
                    while( c < objInfo::MAXCHILDS && (*objiter).second.objChilds[c] != NOUID ){
                        m_constrGroups[ng].constraints[construid].objs.push_back((*objiter).second.objChilds[c]);
                        // fix subObj
                        m_objects[(*objiter).second.objChilds[c++]].obj->fixed = true;
                    }
                    // fix
                    (*objiter).second.constrs.push_back(construid);
                    (*objiter).second.obj->fixed = true;
                    added = true;
                }
            }
        }
        break;
    }
    case CT_COINCIDENT:{
        if (uids.size() == 2 && grouped[OT_POINT].size() == 2 ) { // Distance between 2 points
            ptrep* pt0 = dynamic_cast<ptrep*>( m_objects[ uids[0] ].obj );
            ptrep* pt1 = dynamic_cast<ptrep*>( m_objects[ uids[1] ].obj );
            constrInfo cinfo(type, {new ConstrEqual(pt0->x,pt1->x),new ConstrEqual(pt0->y,pt1->y)},uids );//CT_COINCIDENT;

            constrainEntities({uids[0], uids[1]}, cinfo, puid);
            MOOLOG << "Core::tryAddConstraint - added coincident for "<< *pt0 << " and " << *pt1 << std::endl;
            added = true;
        }
    }
    case CT_DISTANCE:
    case CT_DIMENSION:{
        if ( param < 0) return RC_INVALIDARG;

        if (uids.size() == 2 && grouped[OT_POINT].size() == 2 ) { // Distance between 2 points
            ptrep* pt0 = dynamic_cast<ptrep*>( m_objects[ uids[0] ].obj );
            ptrep* pt1 = dynamic_cast<ptrep*>( m_objects[ uids[1] ].obj );
            _params.push_back(param);
            constrInfo cinfo(type,{new ConstrP2PDist(*pt0,*pt1,&_params.back())},uids);
            constrainEntities({uids[0], uids[1]}, cinfo, puid);
            MOOLOG << "Core::tryAddConstraint - added distance " << param << " between points " << *pt0 << " and " << *pt1 << std::endl;
            added = true;
        } else if (uids.size() == 2 && grouped[OT_POINT].size() == 1 && grouped[OT_SEGMENT].size() == 1 ) { // Distance between point and line
            ptrep* pt = dynamic_cast<ptrep*>(m_objects[grouped[OT_POINT][0]].obj);
            lirep* line = dynamic_cast<lirep*>(m_objects[grouped[OT_SEGMENT][0]].obj);
            _params.push_back(param);
            constrInfo cinfo(type,{ new ConstrP2LDist(*pt,*line,&_params.back())},uids);
            constrainEntities({uids[0], uids[1]}, cinfo, puid);
            MOOLOG << "Core::tryAddConstraint - added distance "<<param << " between point " << *pt << " and line " << *line << std::endl;
            added = true;
        }
        else if ( uids.size() == 2 && grouped[OT_SEGMENT].size() ==2) { // Distance between 2 parallel segments
            int ng = findConstrGroupByObjID(uids[0]);
            // verify we have parallel segments
            if ( ng < 0 ){
                MOOLOG << "Core::tryAddConstraint: unhandled segment(s) dimension|distance constraint " << std::endl;
                added = false;
                break;
            }
            else{
                if ( m_constrGroups[ng].areParallel(uids) ){
                    _params.push_back(param);
                    lirep* line1 = dynamic_cast<lirep*>(m_objects[uids[0]].obj);
                    lirep* line2 = dynamic_cast<lirep*>(m_objects[uids[1]].obj);
                    constrInfo cinfo(type,{ new ConstrP2LDist( *(line1->beg), *line2,&_params.back())},uids);
                    constrainEntities({uids[0], uids[1]}, cinfo, puid);
                    MOOLOG << "Core::tryAddConstraint - added distance "<<param << " between line " << *line1 << " and line " << *line2 <<  std::endl;
                    added = true;
                }

            }
        }
        else if ( uids.size() == grouped[OT_CIRCLE].size()) {
            for (size_t k = 0; k < grouped[OT_CIRCLE].size(); ++k) {
                circrep* ci = dynamic_cast<circrep*>(m_objects[grouped[OT_CIRCLE][k]].obj);
                _const_params.insert(ci->r);
                *(ci->r) = param;
                MOOLOG << "Core::tryAddConstraint - added radius" <<param << " for circle (" <<  *(ci->center) <<","<< *(ci->r) << ")"<< std::endl;
            }
            /*
            UID uid = NOUID;
            if ( puid != nullptr && *puid != NOUID )
                uid = *puid;
            else
                *puid = uid = UIDGen::instance()->generate();

            _constraints[uid] = cinfo;

            cinfo.objs = uids;
            _objects[uids[0]].constrs.push_back(uid);
            */
            added = true;
        }
        break;
    }



    case CT_MIDPOINT:{
        if (uids.size() == 2 && grouped[OT_POINT].size() == 1 && grouped[OT_SEGMENT].size() == 1 ) { // Distance between point and line
            _params.push_back(param);
            ptrep* pt = dynamic_cast<ptrep*>(m_objects[grouped[OT_POINT][0]].obj);
            lirep* line = dynamic_cast<lirep*>(m_objects[grouped[OT_SEGMENT][0]].obj);
            constrInfo cinfo(type,{new ConstrPOnL(*pt,*line),new ConstrMidVal(pt->x,line->beg->x,line->end->x),new ConstrMidVal(pt->y,line->beg->y,line->end->y)},uids);
            constrainEntities({uids[0], uids[1]}, cinfo, puid);
            MOOLOG << "Core::tryAddConstraint - added midpoint for point " << *pt << " and line " << *line << std::endl;
            added = true;
        }
        break;
    }
    case CT_ORTHO:{
        if ( uids.size() == 2 && grouped[OT_SEGMENT].size() == 2 ) {
            lirep* line0 = dynamic_cast<lirep*>(m_objects[uids[0]].obj);
            lirep* line1 = dynamic_cast<lirep*>(m_objects[uids[1]].obj);
            constrInfo cinfo(type, {new ConstrL2LOrtho2(*line0,*line1)},uids);//CT_ORTHO;
            constrainEntities({uids[0],uids[1]},cinfo,puid);
            MOOLOG << "Core::tryAddConstraint - added ortho between line " << *line0 << " and line " << *line1 << std::endl;
            added = true;
        }
        break;
    }
    case CT_PARALLEL:{
        if ( uids.size() == grouped[OT_SEGMENT].size()  && grouped[OT_SEGMENT].size() > 1 ) {
            for (size_t k= 0; k < grouped[OT_SEGMENT].size()-1; ++k) {
                lirep* line0 = dynamic_cast<lirep*>(m_objects[uids[k]].obj);
                lirep* line1 = dynamic_cast<lirep*>(m_objects[uids[k+1]].obj);
                constrInfo cinfo(type,{new ConstrL2LParal(*line0,*line1)},{uids[k],uids[k+1]} );//CT_PARALLEL;
                constrainEntities({uids[k],uids[k+1]},cinfo,puid);
                MOOLOG << "Core::tryAddConstraint - added parallel between line " << *line0 << " and line " << *line1 << std::endl;
            }
            added = true;
        }
        break;
    }
    case CT_VERTICAL:{
        if ( uids.size() == grouped[OT_SEGMENT].size() ) {
            for (size_t k= 0; k < uids.size(); ++k) {
                lirep* li = dynamic_cast<lirep*>(m_objects[uids[k]].obj);
                constrInfo cinfo(type,{new ConstrEqual(li->beg->x,li->end->x)},{uids[k]});//CT_VERTICAL;
                constrainEntities({uids[k]},cinfo,puid);
                MOOLOG << "Core::tryAddConstraint - added " << ((type == CT_VERTICAL)?"vertical":"horizontal")<< " for line " << *li << std::endl;
            }
            added = true;
        }
        break;
    }
    case CT_HORIZONTAL:{
        if ( uids.size() == grouped[OT_SEGMENT].size() ) {
            for (size_t k= 0; k < uids.size(); ++k) {
                lirep* li = dynamic_cast<lirep*>(m_objects[uids[k]].obj);
                constrInfo cinfo(type,{new ConstrEqual(li->beg->y,li->end->y)},{uids[k]});//CT_VERTICAL;
                constrainEntities({uids[k]},cinfo,puid);
                MOOLOG << "Core::tryAddConstraint - added " << ((type == CT_VERTICAL)?"vertical":"horizontal")<< " for line " << *li << std::endl;
            }
            added = true;
        }
        break;
    }
    case CT_COLLINEAR:{
        if ( uids.size() >1  && uids.size() == grouped[OT_SEGMENT].size()  ) {
            for (size_t k= 0; k < uids.size()-1; ++k) {
                lirep* line0 = dynamic_cast<lirep*>(m_objects[uids[k]].obj);
                lirep* line1 = dynamic_cast<lirep*>(m_objects[uids[k+1]].obj);
                constrInfo cinfo(type,{new ConstrL2LParal(*line0,*line1),new ConstrPOnL(*(line0->beg),*line1)},{uids[k],uids[k+1]});//CT_COLLINEAR;
                constrainEntities({uids[k],uids[k+1]},cinfo,puid);
                MOOLOG << "Core::tryAddConstraint - added collinear between line " << *line0 << " and line " << *line1 << std::endl;
            }
            added = true;
        }
        break;
    }
    case CT_TANGENT:{
        if ( !grouped[OT_SEGMENT].empty() ) {
            if ( grouped[OT_CIRCLE].size() == 1 ){
                circrep* ci = dynamic_cast<circrep*>(m_objects[grouped[OT_CIRCLE][0]].obj);
                for (size_t k = 0; k< grouped[OT_SEGMENT].size();++k){
                    lirep* li = dynamic_cast<lirep*>(m_objects[grouped[OT_SEGMENT][k]].obj);

                    constrInfo cinfo(type, {new ConstrP2LDist(*(ci->center),*(li),ci->r,true)} ,{grouped[OT_CIRCLE][0],grouped[OT_SEGMENT][k]});
                    constrainEntities({grouped[OT_CIRCLE][0],grouped[OT_SEGMENT][k]},cinfo,puid);
                    MOOLOG << "Core::tryAddConstraint - added tangent for line " << *li<< " and circle ("<< *(ci->center) <<","<< *(ci->r) << ")"<< std::endl;
                    added = true;
                }
            }
            if ( grouped[OT_ARC].size() == 1 ){
                arcrep* arc = dynamic_cast<arcrep*>(m_objects[grouped[OT_ARC][0]].obj);
                for (size_t k = 0; k< grouped[OT_SEGMENT].size();++k){
                    lirep* li = dynamic_cast<lirep*>(m_objects[grouped[OT_SEGMENT][k]].obj);
                    constrInfo cinfo(type,{new ConstrP2PLDist(*(arc->center),*(arc->beg),*(li))},{grouped[OT_ARC][0],grouped[OT_SEGMENT][k]});//CT_TANGENT;
                    constrainEntities({grouped[OT_ARC][0],grouped[OT_SEGMENT][k]},cinfo,puid);

                    MOOLOG << "Core::tryAddConstraint - added tangent for line " << *li<< " and arc ("<< *(arc->center) <<","<< *(arc->beg)<<","<< *(arc->end) << ")"<< std::endl;
                    added = true;
                }

            }
        }
        break;
    }
    case CT_EQUAL:{
        if ( uids.size() > 1 ){
            if ( uids.size() == grouped[OT_CIRCLE].size() ) {
                for ( size_t k= 0; k < grouped[OT_CIRCLE].size()-1; ++k ) {
                    circrep* ci0 = dynamic_cast<circrep*>(m_objects[grouped[OT_CIRCLE][k]].obj);
                    circrep* ci1 = dynamic_cast<circrep*>(m_objects[grouped[OT_CIRCLE][k+1]].obj);
                    constrInfo cinfo(type,{new ConstrEqual(ci0->r,ci1->r)},{grouped[OT_CIRCLE][k],grouped[OT_CIRCLE][k+1]});//CT_EQUAL;
                    constrainEntities({grouped[OT_CIRCLE][k],grouped[OT_CIRCLE][k+1]},cinfo,puid);

                    MOOLOG << "Core::tryAddConstraint - added equal radius for circles " << *ci0->center << " and " << *ci1->center << std::endl;
                    added = true;
                }
            }
            if ( uids.size() == grouped[OT_SEGMENT].size() ) {
                for (size_t k= 0; k < grouped[OT_SEGMENT].size()-1; ++k) {
                    lirep* line0 = dynamic_cast<lirep*>(m_objects[grouped[OT_SEGMENT][k]].obj);
                    lirep* line1 = dynamic_cast<lirep*>(m_objects[grouped[OT_SEGMENT][k+1]].obj);

                    constrInfo cinfo(type,{new ConstrL2LEqual(*line0,*line1)},{grouped[OT_SEGMENT][k], grouped[OT_SEGMENT][k+1]});//CT_EQUAL;
                    constrainEntities({grouped[OT_SEGMENT][k], grouped[OT_SEGMENT][k+1]},cinfo,puid);
                    MOOLOG << "Core::tryAddConstraint - added equal for segments " << *line0 << " and " << *line1 << std::endl;

                }
                added = true;
            }
            if ( grouped[OT_POINT].size() == 4 ) {
                for (size_t k= 0; k < grouped[OT_POINT].size()-1; k+=4) {
                    ptrep* pb1 = dynamic_cast<ptrep*>(m_objects[grouped[OT_POINT][k]].obj);
                    ptrep* pe1 = dynamic_cast<ptrep*>(m_objects[grouped[OT_POINT][k+1]].obj);
                    ptrep* pb2 = dynamic_cast<ptrep*>(m_objects[grouped[OT_POINT][k+2]].obj);
                    ptrep* pe2 = dynamic_cast<ptrep*>(m_objects[grouped[OT_POINT][k+3]].obj);

                    constrInfo cinfo(type,{new ConstrL2LEqual(*pb1,*pe1,*pb2,*pe2)},{grouped[OT_POINT][k],grouped[OT_POINT][k+1],grouped[OT_POINT][k+2],grouped[OT_POINT][k+3]});//CT_EQUAL;
                    constrainEntities({grouped[OT_POINT][k],grouped[OT_POINT][k+1],grouped[OT_POINT][k+2],grouped[OT_POINT][k+3]},cinfo,puid);

                    MOOLOG << "Core::tryAddConstraint - added equal for points " << *pb1 << " and " << *pe1 << " and " << *pb2 << " and " << *pe2 << std::endl;

                }
                added = true;
            }            
        }
        break;
    }
    case CT_CONCENTRIC:{
        std::map<ObjectType,std::vector<UID> > grouped;
        groupObjUIDsByType(uids,grouped);
        if ( uids.size() > 1 ){
            if ( uids.size() == grouped[OT_CIRCLE].size() ) {
                for ( size_t k= 0; k < grouped[OT_CIRCLE].size()-1; ++k ) {
                    circrep* ci0 = dynamic_cast<circrep*>(m_objects[grouped[OT_CIRCLE][k]].obj);
                    circrep* ci1 = dynamic_cast<circrep*>(m_objects[grouped[OT_CIRCLE][k+1]].obj);
                    constrInfo cinfo(type,{new ConstrEqual(ci0->center->x,ci1->center->x),new ConstrEqual(ci0->center->y,ci1->center->y)},{ grouped[OT_CIRCLE][k] , grouped[OT_CIRCLE][k+1]} );//CT_CONCENTRIC;
                    constrainEntities( {grouped[OT_CIRCLE][k],grouped[OT_CIRCLE][k+1]}, cinfo,puid );

                    MOOLOG << "Core::tryAddConstraint - added concentric for circles " << *ci0->center << " and " << *ci1->center << std::endl;
                }
                added = true;
            }
        }
        break;
    }


    default:
        MOOLOG << "Core:: Unhandled constraint type" << std::endl;

    }
    if (added) return RC_OK;
    return RC_RUNTIME_ERR;
}

RESCODE georis::Core::addConstraint(ConstraintType type,const std::vector<UID> &uids,double param,UID *puid) {
    if ( uids.empty() ) return RC_INVALIDARG;

    //check for uids not ours or child object uids of any given
    for (size_t k = 0;k< uids.size();++k){
        auto it = m_objects.find(uids[k]);
        if ( it == m_objects.end() ) return RC_NO_OBJ;
        // We can't apply constraint to parent and child objects together
        size_t c = 0;
        while (c < objInfo::MAXCHILDS && (*it).second.objChilds[c] != NOUID){
            for (size_t k1 = 0;k1< uids.size();++k1)
                if (uids[k] == (*it).second.objChilds[c]) return -1;
            ++c;
        }
    }

    MOOLOG << "Core::addConstraint " << uids.size() << " objects" << std::endl;

    backupState();
    RESCODE res = tryAddConstraint(type,uids,param,puid);
    MOOLOG << "Core::addConstraint " << res << " tac" << std::endl;
    if ( RC_OK != res ) return res;
    res = solve();
    if ( RC_OK == res ) return res;
    restoreState();
    return res;
}
RESCODE georis::Core::removeConstraint(UID id2rem) {

    int ng = findConstrGroupByConstrID(id2rem);
    if ( ng < 0 ) return RC_NO_OBJ;
    // check if this constraint is the link in group
    if ( 0/*m_constrGroups[ng].isLink(id2rem)*/ ){
        //divide group
    }
    else{
        auto cit = m_constrGroups[ng].constraints.find(id2rem);
        // clear constraint from linked objects
        for (size_t k = 0;k < (*cit).second.objs.size();++k)
            for (size_t m = 0;m < m_objects[(*cit).second.objs[k]].constrs.size();++m )
                if ( m_objects[(*cit).second.objs[k]].constrs[m] == id2rem ){
                    m_objects[(*cit).second.objs[k]].constrs.erase(m_objects[(*cit).second.objs[k]].constrs.begin()+m);
                    break;
                }
        // clear _constraints
        m_constrGroups[ng].constraints.erase(cit);
    }

    return RC_OK;
}
RESCODE georis::Core::queryConstrInfo(UID uid,ConstraintType &type,std::vector<UID> &objs,double *pparam)const {
    int ng = findConstrGroupByConstrID(uid);
    if ( ng < 0 ) return RC_NO_OBJ;
    for ( auto constrgr:m_constrGroups ){
        type = constrgr.constraints[uid].type;
        objs.clear();
        objs = constrgr.constraints[uid].objs;
    }

    return RC_OK;
}
void georis::Core::enumConstraints(std::vector<UID>&uids)const{
    uids.clear();
    for (auto grit : m_constrGroups){
        for ( auto cit: grit.constraints )
            uids.push_back(cit.first);
    }
}
void georis::Core::groupObjUIDsByType(const std::vector<UID> &uids, std::map<ObjectType,std::vector<UID> > &grouped)const{
    grouped.clear();
    for (auto id : uids ){
        auto objit = m_objects.find( id );
        assert( objit != m_objects.end() );
        grouped[(*objit).second.obj->type()].push_back( id );
    }

    for (auto it = grouped.begin();it != grouped.end();++it){
        MOOLOG << "Core::groupObj type = ";
        switch  ( (*it).first ){
        case OT_POINT:
            MOOLOG << "point";
            break;
        case OT_SEGMENT:
            MOOLOG << "segment";
            break;
        case OT_CIRCLE:
            MOOLOG << "circle";
            break;
        case OT_ARC:
            MOOLOG << "arc";
            break;
        default :
            MOOLOG << " ACHTUNG !!!";
        }
        MOOLOG << " count " << (*it).second.size() << std::endl;
    }
}
int georis::Core::solve(){
    try{
        auto t1 = Clock::now();
        for (auto& cgroup: m_constrGroups ){
            if ( cgroup.unsolved ){
                std::vector<double*> params = cgroup.getTunableParams();
                //MOOLOG << "Core::solve - there are "<< params.size() << " params total" << std::endl;
                //removeFixedParameters(params);
                MOOLOG << "Core::solve - there are "<< params.size() << " tunable params" << std::endl;
                MOOLOG << "Core::solve - there are "<< cgroup.constraints.size() << " constraints" << std::endl;

                // Create function for constraints group
                GeosFuncN func(&params);

                for ( auto &c: cgroup.constraints ){
                    for (auto &e: c.second.errors)
                        func.add(e);
                }

                SolveTask geotask;
                geotask.target = &func;

                geotask.x0 = v_type(params.size());
                for (size_t k = 0; k<params.size(); ++k)
                    geotask.x0(k) = *params[k];
                v_type err = func(geotask.x0);
                MOOLOG << "Core::solve - initial error = " <<std::endl << err << std::endl << ", norm = " << (err.transpose()*err) << std::endl;
                MOOLOG << "Core::solve - at " << std::endl << geotask.x0 << std::endl;
                geotask.stopcond.tolx = 1e-8;
                geotask.stopcond.tolf = 1e-8;
                geotask.stopcond.fevals = 10000;

                //SolverCeres solver;
                //SolverLM solver;
                SolverNG solver;

                solver.solve(geotask);

                for (size_t k = 0; k<params.size(); ++k)
                    geotask.x0(k) = *params[k];
                err = func(geotask.x0);
                MOOLOG << "Core::solve - final error = "<< std::endl << err << std::endl  << ", norm = " << (err.transpose()*err) << std::endl;
                MOOLOG << "Core::solve - at " << std::endl << geotask.x0 << std::endl;
                //MOOLOG << "=============================" << std::endl;
                cgroup.unsolved = false;
            }
        }
        //MOOLOG << "=============================" << std::endl;
        auto t2 = Clock::now();
        MOOLOG << "Core::solve solved at " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << std::endl;
    }
    catch (std::runtime_error &e){
        MOOLOG << "Core::solve: " << e.what() << std::endl;
        return RC_NOTSOLVED;
    }

    return 0;
}
void georis::Core::removeFixedParameters(std::vector<double*>& param ){
    std::set<double*> setpara;
    setpara.insert(param.begin(),param.end());
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it){
        if ( (*it).second.obj->fixed ) {
            switch ((*it).second.obj->type()){
            case OT_POINT:{
                ptrep *pt = dynamic_cast<ptrep*>((*it).second.obj);
                setpara.erase( pt->x) ;
                setpara.erase( pt->y) ;
                break;
            }
            case OT_SEGMENT:{
                lirep *li = dynamic_cast<lirep*>((*it).second.obj);
                setpara.erase( li->beg->x) ;
                setpara.erase( li->beg->y);
                setpara.erase( li->end->x) ;
                setpara.erase( li->end->y);
                break;
            }
            case OT_CIRCLE:{
                circrep *ci = dynamic_cast<circrep*>((*it).second.obj);
                setpara.erase( ci->center->x);
                setpara.erase( ci->center->y);
                setpara.erase( ci->r);
                break;
            }
            default:
                ;
            }
        }
    }
    param.clear();
    param = std::vector<double*>(setpara.begin(),setpara.end());
}
RESCODE georis::Core::moveObjects(const std::vector<UID>& objs,double dx, double dy){
    auto objsnochi(objs);
    filterChildObj(objsnochi);
    MOOLOG << "Core::moveObjects moving " << objsnochi.size() << std::endl;
    backupState();
    for (auto v:objsnochi){
        auto it = m_objects.find(v);
        if ( it != m_objects.end() ){
            (*it).second.obj->move(dx,dy);
            int ng = findConstrGroupByObjID(it->first);
            if ( ng > -1 ) m_constrGroups[ng].unsolved = true;
        }
    }
    int res = solve();
    if ( 0 == res ) return res;
    MOOLOG << "Core::moveObjects infeasible move, restoring " << std::endl;
    restoreState();
    return res;
}
void georis::Core::filterChildObj(std::vector<UID> &objs)const{
    size_t k = 0;
    while (k < objs.size() ){
        UID uidpar = NOUID;
        for ( auto obj : m_objects )
            if ( obj.second.hasChild(objs[k]) ) {
                uidpar = obj.first;
                break;
            }

        if ( uidpar != NOUID )
            for (size_t m = 0;m<objs.size();++m)
                if ( uidpar == objs[m] ){
                    objs.erase(objs.begin()+k);
                    --k;
                    break;
                }
        ++k;
    }

}

int georis::Core::findConstrGroupByConstrID(UID constrID)const{
    for (size_t k = 0;k < m_constrGroups.size();++k){
        if ( m_constrGroups[k].constraints.end() != m_constrGroups[k].constraints.find(constrID) ) return k;
    }
    return -1;
}
int georis::Core::findConstrGroupByObjID(UID objID)const{
    for (size_t k = 0;k < m_constrGroups.size();++k){
        for ( auto &constr: m_constrGroups[k].constraints )
            for ( auto cobjID:constr.second.objs )
                if (cobjID == objID )
                    return k;
    }
    return -1;
}
std::vector<double*> georis::Core::constrGroup::getAllParams()const{
    std::set<double*> sorter;
    for ( auto& constr: constraints )
        for ( auto& csc: constr.second.errors )
            sorter.insert(csc->cparam().begin(),csc->cparam().end() );
    std::vector<double*>  res;
    res.reserve(sorter.size());
    for (auto par:sorter)
        res.push_back(par);
    return res;
}
std::vector<double*> georis::Core::constrGroup::getTunableParams()const{
    std::set<double*> sorter;
    std::vector<double*> fixed;
    for ( auto& constr: constraints ){
        for ( auto& csc: constr.second.errors )
            sorter.insert(csc->cparam().begin(),csc->cparam().end() );
        if (constr.second.type == CT_FIX){
            for ( auto objid : constr.second.objs ){

            }
            // fill in fixed parameters
        }
    }
    std::vector<double*>  res;
    std::set_difference(sorter.begin(),sorter.end(),fixed.begin(),fixed.end(),std::back_inserter(res));
    return res;
}
bool georis::Core::constrGroup::areParallel(const std::vector<UID> &segmentsinq){
    if ( segmentsinq.size() < 2) return false;

    // Filter only parallel constraints and find objects constrained by them
    struct parconstrinfo{
        bool used;
        UID first;
        UID second;
    };
    std::map<UID,parconstrinfo> objByParConstr;
    struct parseginfo{
        bool used;
        std::vector<UID> constrs;
    };
    std::map<UID,parseginfo > parConstrByObj;
    for (auto & constr : constraints ){
        if ( constr.second.type == CT_PARALLEL ){
            //parallelConstrs.push_back(constr.first);
            objByParConstr[constr.first] = {false,constr.second.objs[0],constr.second.objs[1]};
            parConstrByObj[constr.second.objs[0]].used = false;
            parConstrByObj[constr.second.objs[0]].constrs.push_back(constr.first);
            parConstrByObj[constr.second.objs[1]].constrs.push_back(constr.first);
        }
    }
    if ( parConstrByObj.size() < segmentsinq.size() || objByParConstr.size() < segmentsinq.size() - 1 ) return false;

    std::stack<UID> c2do;
    c2do.push(objByParConstr.begin()->first);
    size_t usedConstrs = 0;
    std::set<UID> constrained;
    while( usedConstrs != objByParConstr.size() ){
        UID currconstr = c2do.top();
        c2do.pop();
        ++usedConstrs;
        // Пометить ограничение как использованное
        objByParConstr[currconstr].used = true;
        // Добавить в множество ограниченных отрезков отрезки, связанные данным ограничением
        constrained.insert(objByParConstr[currconstr].first);
        constrained.insert(objByParConstr[currconstr].second);
        // Для всех отрезков, участвующих в данном ограничении
        // взять все неиспользованные ограничения, связанные с данными отрезками и положить в стек
        for ( auto constrid: parConstrByObj[objByParConstr[currconstr].first].constrs ){
            if ( !objByParConstr[constrid].used )
                c2do.push(constrid);
        }
        for ( auto constrid: parConstrByObj[objByParConstr[currconstr].second].constrs ){
            if ( !objByParConstr[constrid].used )
                c2do.push(constrid);
        }
        //
        if ( c2do.empty() ){
            if ( constrained.find(segmentsinq[0]) != constrained.end() ){
                if ( constrained.size() < segmentsinq.size() )
                    return false;
                bool notFound = false;
                for (size_t k = 1;k < segmentsinq.size();++ k )
                    if ( constrained.find(segmentsinq[k]) == constrained.end() ){
                        notFound = true;
                        break;
                    }
                if ( notFound ) return false;
                return true;
            }
            else{
                constrained.clear();
                // Найдем неиспользованное ограничение
                for ( auto co: objByParConstr ){
                    if ( ! co.second.used ){
                        c2do.push( co.first );
                        break;
                    }
                }
            }
        }
    }
    return false;
}
void georis::Core::mergeConstrGroups(int ng1,int ng2){
    assert(0<=ng1 && ng1 < m_constrGroups.size());
    assert(0<=ng2 && ng2 < m_constrGroups.size());
    if ( ng1 == ng2 ) return;
    int mi = std::min(ng1,ng2);
    int ma = std::max(ng1,ng2);
    for ( auto constr: m_constrGroups[ma].constraints )
        m_constrGroups[mi].constraints[constr.first] = constr.second;
    m_constrGroups.erase(m_constrGroups.begin() + ma);
    m_constrGroups[mi].unsolved = true;
}
/*
void georis::Core::constrainTwo(UID objuid0, UID objuid1, constrInfo& cinfo, UID* puid){
    UID construid = NOUID;
    if ( puid != nullptr ){
        if (*puid != NOUID )
            construid = *puid;
        else
            *puid = construid = UIDGen::instance()->generate();
    }
    else
        construid = UIDGen::instance()->generate();

    cinfo.objs.push_back(objuid0);
    cinfo.objs.push_back(objuid1);

    std::vector<UID> chuids0;
    getObjChilds(objuid0,chuids0);
    cinfo.objs.insert(cinfo.objs.end(),chuids0.begin(),chuids0.end());
    std::vector<UID> chuids1;
    getObjChilds(objuid1,chuids1);
    cinfo.objs.insert(cinfo.objs.end(),chuids1.begin(),chuids1.end());

    int ng0 = 0;
    UID uidpar0 = NOUID;
    getObjParent(objuid0,uidpar0);
    if ( uidpar0 == NOUID )
        ng0 = findConstrGroupByObjID(objuid0);
    else
        ng0 = findConstrGroupByObjID(uidpar0);

    int ng1 = 0;
    UID uidpar1 = NOUID;
    getObjParent(objuid1,uidpar1);
    if ( uidpar1 == NOUID )
        ng1 = findConstrGroupByObjID(objuid1);
    else
        ng1 = findConstrGroupByObjID(uidpar1);

    if ( ng0 < 0 ){
        if ( ng1 < 0 ) { // Add new group
            m_constrGroups.push_back(constrGroup());
            m_constrGroups.back().constraints[construid] = cinfo;
        }
        else{ // Add to ng1
            m_constrGroups[ng1].constraints[construid] = cinfo;
            m_constrGroups[ng1].unsolved = true;
        }
    }
    else{
        m_constrGroups[ng0].constraints[construid] = cinfo;
        m_constrGroups[ng0].unsolved = true;

        if ( ng1 >= 0 ) { // Merge ng0 and ng1
            mergeConstrGroups(ng0,ng1);
        }
    }

    m_objects[objuid0].constrs.push_back(construid);
    if ( uidpar0 != NOUID )
        m_objects[uidpar0].constrs.push_back(construid);
    m_objects[objuid1].constrs.push_back(construid);
    if ( uidpar1 != NOUID )
        m_objects[uidpar1].constrs.push_back(construid);


}
*/
void georis::Core::constrainEntities(const std::vector<UID>& objuids, constrInfo& cinfo, UID* puid){
    assert(objuids.size() < 3);
    // Generate new constraint ID
    UID construid = NOUID;
    if ( puid != nullptr ){
        if (*puid != NOUID )
            construid = *puid;
        else
            *puid = construid = UIDGen::instance()->generate();
    }
    else
        construid = UIDGen::instance()->generate();

    for ( auto objuid: objuids ){
        cinfo.objs.push_back( objuid );
        std::vector<UID> chuids;
        getObjChilds(objuid,chuids);
        cinfo.objs.insert(cinfo.objs.end(),chuids.begin(),chuids.end());
    }

    int ng0 = -1;
    UID uidpar0 = NOUID;
    std::vector<UID> paruids;
    getObjParent(objuids[0],uidpar0);
    if ( uidpar0 == NOUID )
        ng0 = findConstrGroupByObjID(objuids[0]);
    else{
        ng0 = findConstrGroupByObjID(uidpar0);
        paruids.push_back(uidpar0);
    }

    int ng1 = -1;
    if ( objuids.size() > 1 ){
        UID uidpar1 = NOUID;
        getObjParent(objuids[1],uidpar1);
        if ( uidpar1 == NOUID )
            ng1 = findConstrGroupByObjID(objuids[1]);
        else{
            ng1 = findConstrGroupByObjID(uidpar1);
            paruids.push_back(uidpar1);
        }
    }

    if ( ng0 < 0 ){
        if ( ng1 < 0 ) { // Add new group
            m_constrGroups.push_back(constrGroup());
            m_constrGroups.back().constraints[construid] = cinfo;
        }
        else{ // Add to ng1
            m_constrGroups[ng1].constraints[construid] = cinfo;
            m_constrGroups[ng1].unsolved = true;
        }
    }
    else{
        m_constrGroups[ng0].constraints[construid] = cinfo;
        m_constrGroups[ng0].unsolved = true;

        if ( ng1 >= 0 ) { // Merge ng0 and ng1
            mergeConstrGroups(ng0,ng1);
        }
    }

    for ( auto objuid : objuids )
        m_objects[objuid].constrs.push_back(construid);
    for ( auto paruid : paruids )
        m_objects[paruid].constrs.push_back(construid);
}

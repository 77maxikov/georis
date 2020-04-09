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
}
RESCODE georis::Core::setActiveSketch(UID){

}
RESCODE georis::Core::removeSketch(UID){

}

RESCODE georis::Core::addObject(georis::ObjectType type, const std::vector<double>&param,UID *puid,std::vector<UID> *pchuids) {
    // validate uid if given
    if ( puid != nullptr && *puid != NOUID && _objects.find(*puid) != _objects.end() ) return RC_NOTUNIQUE;

    switch (type) {
    case OT_POINT: {        
        if (param.size() != 2 ) return RC_INVALIDARG;
        if (puid){
            if ( *puid == NOUID )   *puid = internalAddPoint(param[0],param[1]);
            else{
                _params.push_back(param[0]);
                double *px = &_params.back();
                _params.push_back(param[1]);
                double *py = &_params.back();
                objInfo info;
                ptrep * pt = new ptrep(px,py);
                info.obj = pt;
                _objects[*puid] = info;
            }
        }
        else
            internalAddPoint(param[0],param[1]);
        break;
    }
    case OT_SEGMENT: {
        if ( pchuids != nullptr ){
            if ( pchuids->size() != 2 ) return RC_INVALIDARG;
            // validate if children are points
            auto ptit0 = _objects.find((*pchuids)[0]);
            if ( ptit0 == _objects.end() ) return RC_NO_OBJ;
            if ( (*ptit0).second.obj->type() != OT_POINT ) return RC_INVALIDARG;
            auto ptit1 = _objects.find((*pchuids)[1]);
            if ( ptit1 == _objects.end() ) return RC_NO_OBJ;
            if ( (*ptit1).second.obj->type() != OT_POINT ) return RC_INVALIDARG;

            objInfo info;
            info.obj = new lirep( dynamic_cast<ptrep*>( (*ptit0).second.obj ), dynamic_cast<ptrep*>( (*ptit1).second.obj ));
            info.objChilds[0] = (*pchuids)[0];
            info.objChilds[1] = (*pchuids)[1];

        }
        else
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
        _objects[uid] = info;
        if (puid) *puid = uid;

        break;
    }
    case OT_CIRCLE: {
        ptrep* pt = 0;
        UID uidp = internalAddPoint(param[0],param[1],&pt);
        _params.push_back(param[2]);
        UID uid = UIDGen::instance()->generate();
        objInfo info;
        info.obj = new circrep(pt,&_params.back());
        info.objChilds[0] = uidp;
        _objects[uid] = info;
        if (puid) *puid = uid;
		break;
    }
    case OT_ARC:{
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

        _objects[uid] = info;
        if (puid) *puid = uid;

        RESCODE res = addConstraint(CT_EQUAL,{uidc,uidp1,uidc,uidp2});

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
    _objects[uid] = info;
    if ( res ) *res = pt;
    return uid;
}

void georis::Core::internalRemovePoint(UID pui) {
    auto objit = _objects.find(pui);
    if ( objit == _objects.end()) return;

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
    _objects.erase(objit);
}

RESCODE georis::Core::removeObject(UID remUID) {
    MOOLOG << "Core::removeObject: before - objects " << _objects.size() << " parameters " << _params.size() << std::endl;
    auto objit = _objects.find(remUID);
    if ( objit == _objects.end() ) return RC_NO_OBJ;
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
        _objects.erase(objit);
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
        _objects.erase(objit);
        break;
    }
    case OT_POINT: {
        // Check if point is not standalone
        for (auto cit = _objects.begin(); cit != _objects.end(); ++cit)
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
        _objects.erase(objit);
        break;
    }
    case OT_SPLINE:
    case OT_NONE:
    default:
        ;
    }
    MOOLOG << "Core::removeObject: after - objects " << _objects.size() << " parameters " << _params.size() << std::endl;
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
	auto objit = _objects.find(uid);
	if ( objit == _objects.end() ) return RC_NO_OBJ;

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
    uids.reserve(_objects.size());
    for (auto it : _objects){
        uids.push_back(it.first);
    }
}
RESCODE georis::Core::getObjChilds(UID uid,std::vector<UID>&uids)const {
	uids.clear();
    auto objit = _objects.find(uid);
    if ( objit == _objects.end() )
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
    for ( auto obj : _objects)
        if ( obj.second.hasChild(uid) ) { par = obj.first; return RC_OK;}
    par = NOUID;
    return RC_NO_OBJ;
}
RESCODE georis::Core::getObjConstraints(UID uid,std::vector<UID>& res)const {
	res.clear();
    auto objit = _objects.find(uid);
    if ( objit == _objects.end() ) return RC_NO_OBJ;
	res = (*objit).second.constrs;
	return RC_OK;
}
void georis::Core::findObjInCirc(const point2r&p,double radius,std::vector<UID> &uids,std::vector<double> *dists)const{
	uids.clear();
	for (auto it = _objects.begin();it != _objects.end();++it){
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
    for (auto it = _objects.begin();it != _objects.end();++it){
    	if ( (*it).second.obj->inRect(*pLo.x,*pLo.y,*pHi.x,*pHi.y) )
			uids.push_back((*it).first);
    }
}

int georis::Core::backupState(){
	_paramsBU.resize(_params.size());
	std::copy(_params.begin(),_params.end(),_paramsBU.begin());
	_constraintsBU = _constraints;
	return 0;
}
int georis::Core::restoreState(){
	if (_params.size() != _paramsBU.size()) return -1;
	std::copy(_paramsBU.begin(),_paramsBU.end(),_params.begin());
	_constraints = _constraintsBU;
	return 0;
}

RESCODE georis::Core::tryAddConstraint(ConstraintType type,const std::vector<UID> &uids,double param,UID *puid) {
    if ( puid != nullptr && *puid != NOUID && _constraints.find(*puid) != _constraints.end() ){
        return RC_INVALIDARG;
    }
	bool added = false;

    switch (type) {
/*    case CT_FIX:{
        for (size_t k = 0;k<uids.size();++k){
			auto it = _objects.find(uids[k]);
			if ( it != _objects.end() ){
				size_t c = 0;
				while( c < objInfo::MAXCHILDS && (*it).second.objChilds[c] != NOUID ){
					// fix subObj
					_objects[(*it).second.objChilds[c]].obj->fixed = true;
					++c;
				}
				// fix
				(*it).second.obj->fixed = true;
			}
        }
        break;
    }*/
    case CT_COINCIDENT:
        param = 0;
    case CT_DISTANCE:
    case CT_DIMENSION:{
        if ( param < 0)
            return RC_INVALIDARG;

		std::map<ObjectType,std::vector<objInfo*> > grouped;
		groupObj(uids,grouped);

        if (uids.size() == 2 && grouped[OT_POINT].size() == 2 ) { // Distance between 2 points
            ptrep* pt0 = dynamic_cast<ptrep*>(grouped[OT_POINT][0]->obj);
            ptrep* pt1 = dynamic_cast<ptrep*>(grouped[OT_POINT][1]->obj);
            if ( param > 0 ) {
                _params.push_back(param);
				constrInfo cinfo;
				cinfo.type = CT_DIMENSION;
				cinfo.constrs.push_back(ceFunc());

				cinfo.constrs[0].constr = new ConstrP2PDist(*pt0,*pt1,&_params.back());
                cinfo.constrs[0].cparam.push_back(pt0->x);
                cinfo.constrs[0].cparam.push_back(pt0->y);
                cinfo.constrs[0].cparam.push_back(pt1->x);
                cinfo.constrs[0].cparam.push_back(pt1->y);
                cinfo.constrs[0].cparam.push_back(&_params.back());

                cinfo.objs = uids;

                UID uid = NOUID;
                if ( puid != nullptr ){
                    if (*puid != NOUID )
                        uid = *puid;
                    else
                        *puid = uid = UIDGen::instance()->generate();
                }
                else
                    uid = UIDGen::instance()->generate();

                _constraints[uid] = cinfo;


                _objects[uids[0]].constrs.push_back(uid);
                _objects[uids[1]].constrs.push_back(uid);

                MOOLOG << "GeosController::tryAddConstraint - added distance " << param << " between points " << *pt0 << " and " << *pt1 << std::endl;
            } else {
                constrInfo cinfo;
                cinfo.type = CT_COINCIDENT;
                cinfo.constrs.resize(2);

                cinfo.constrs[0].constr = new ConstrEqual(pt0->x,pt1->x);
                cinfo.constrs[0].cparam.push_back(pt0->x);
				cinfo.constrs[0].cparam.push_back(pt1->x);

                cinfo.constrs[1].constr = new ConstrEqual(pt0->y,pt1->y);
				cinfo.constrs[1].cparam.push_back(pt0->y);
				cinfo.constrs[1].cparam.push_back(pt1->y);

                cinfo.objs = uids;

                UID uid = NOUID;
                if ( puid != nullptr ){
                    if (*puid != NOUID )
                        uid = *puid;
                    else
                        *puid = uid = UIDGen::instance()->generate();
                }
                else
                    uid = UIDGen::instance()->generate();

                _constraints[uid] = cinfo;

                _objects[uids[0]].constrs.push_back(uid);
                _objects[uids[1]].constrs.push_back(uid);

                MOOLOG << "GeosController::tryAddConstraint - added coincident "<< *pt0 << " and " << *pt1 <<" with uid " << uid <<  std::endl;
            }
            added = true;
        } else if (uids.size() == 2 && grouped[OT_POINT].size() == 1 && grouped[OT_SEGMENT].size() == 1 ) { // Distance between point and line
			_params.push_back(param);
            ptrep* pt = dynamic_cast<ptrep*>(grouped[OT_POINT][0]->obj);
            lirep* line = dynamic_cast<lirep*>(grouped[OT_SEGMENT][0]->obj);

			constrInfo cinfo;
			cinfo.type = CT_DISTANCE;
			cinfo.constrs.resize(1);

			cinfo.constrs[0].constr = new ConstrP2LDist(*pt,*line,&_params.back());
			cinfo.constrs[0].cparam.push_back(pt->x);
			cinfo.constrs[0].cparam.push_back(pt->y);
			cinfo.constrs[0].cparam.push_back(line->beg->x);
            cinfo.constrs[0].cparam.push_back(line->beg->y);
            cinfo.constrs[0].cparam.push_back(line->end->x);
            cinfo.constrs[0].cparam.push_back(line->end->y);
            cinfo.constrs[0].cparam.push_back(&_params.back());

            cinfo.objs = uids;

            UID uid = NOUID;
            if ( puid != nullptr ){
                if (*puid != NOUID )
                    uid = *puid;
                else
                    *puid = uid = UIDGen::instance()->generate();
            }
            else
                uid = UIDGen::instance()->generate();

			_constraints[uid] = cinfo;

			_objects[uids[0]].constrs.push_back(uid);
			_objects[uids[1]].constrs.push_back(uid);

            MOOLOG << "GeosController::tryAddConstraint - added distance "<<param << " between point " << *pt << " and line " << *line << " with uid " << uid << std::endl;
            added = true;
		}
        else if ( uids.size() == grouped[OT_SEGMENT].size() ) { // Line segment lengths
/*                _params.push_back(param);
                for (size_t k = 0; k < grouped[OT_SEGMENT].size(); ++k) {
                    lirep* line = dynamic_cast<lirep*>(grouped[OT_SEGMENT][k]->obj);

					constrInfo cinfo;
					cinfo.constr =  new ConstrP2PDist(line->beg,line->end,&_params.back());
					cinfo.objs = uids;
					cinfo.cparam.push_back(line->beg->x);
                    cinfo.cparam.push_back(line->beg->y);
                    cinfo.cparam.push_back(line->end->x);
                    cinfo.cparam.push_back(line->end->y);
					cinfo.cparam.push_back(&_params.back());

                    cp[0] = line->beg->x;
                    cp[1] = line->beg->y;
                    cp[2] = line->end->x;
                    cp[3] = line->end->y;

                    IConstraint *constraint = new ConstrP2PDist(*line->beg,*line->end,&_params.back());
                    _con_par[constraint ] = cp;
                    MOOLOG << "GeosController::addConstraint - added length "<<param << " for line " << *line << std::endl;
                }
                added = true;
*/
		}
		else if ( uids.size() == grouped[OT_CIRCLE].size()) {
			for (size_t k = 0; k < grouped[OT_CIRCLE].size(); ++k) {
				circrep* ci = dynamic_cast<circrep*>(grouped[OT_CIRCLE][k]->obj);
                _const_params.insert(ci->r);
                *(ci->r) = param;
                MOOLOG << "GeosController::tryAddConstraint - added radius" <<param << " for circle (" <<  *(ci->center) <<","<< *(ci->r) << ")"<< std::endl;
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
    case CT_ORTHO:{
		std::map<ObjectType,std::vector<objInfo*> > grouped;
		groupObj(uids,grouped);

        if ( uids.size() == 2 && grouped[OT_SEGMENT].size() == 2 ) {
			lirep* line0 = dynamic_cast<lirep*>(grouped[OT_SEGMENT][0]->obj);
			lirep* line1 = dynamic_cast<lirep*>(grouped[OT_SEGMENT][1]->obj);

			constrInfo cinfo;
			cinfo.type = CT_ORTHO;
			cinfo.constrs.resize(1);

			cinfo.objs.push_back(uids[0]);
			cinfo.objs.push_back(uids[1]);
			cinfo.constrs[0].constr = new ConstrL2LOrtho(*line0,*line1);
			cinfo.constrs[0].cparam.push_back(line0->beg->x);
			cinfo.constrs[0].cparam.push_back(line0->beg->y);
			cinfo.constrs[0].cparam.push_back(line0->end->x);
			cinfo.constrs[0].cparam.push_back(line0->end->y);

			cinfo.constrs[0].cparam.push_back(line1->beg->x);
			cinfo.constrs[0].cparam.push_back(line1->beg->y);
			cinfo.constrs[0].cparam.push_back(line1->end->x);
			cinfo.constrs[0].cparam.push_back(line1->end->y);

            UID uid = NOUID;
            if ( puid != nullptr ){
                if (*puid != NOUID )
                    uid = *puid;
                else
                    *puid = uid = UIDGen::instance()->generate();
            }
            else
                uid = UIDGen::instance()->generate();

            cinfo.objs = uids;

			_constraints[uid] = cinfo;

            _objects[uids[0]].constrs.push_back(uid);
            _objects[uids[1]].constrs.push_back(uid);

            MOOLOG << "GeosController::tryAddConstraint - added ortho between line " << *line0 << " and line " << *line1 << " with uid " << uid << std::endl;
            added = true;
        }
        break;
    }
	case CT_PARALLEL:{
    	std::map<ObjectType,std::vector<objInfo*> > grouped;
		groupObj(uids,grouped);
        if ( uids.size() == grouped[OT_SEGMENT].size()  && grouped[OT_SEGMENT].size() > 1 ) {
            for (size_t k= 0; k < grouped[OT_SEGMENT].size()-1; ++k) {
				lirep* line0 = dynamic_cast<lirep*>(grouped[OT_SEGMENT][k]->obj);
				lirep* line1 = dynamic_cast<lirep*>(grouped[OT_SEGMENT][k+1]->obj);

				constrInfo cinfo;
				cinfo.type = CT_PARALLEL;
				cinfo.constrs.resize(1);

				cinfo.objs.push_back(uids[k]);
				cinfo.objs.push_back(uids[k+1]);

				cinfo.constrs[0].constr = new ConstrL2LParal(*line0,*line1);

				cinfo.constrs[0].cparam.push_back(line0->beg->x);
				cinfo.constrs[0].cparam.push_back(line0->beg->y);
				cinfo.constrs[0].cparam.push_back(line0->end->x);
				cinfo.constrs[0].cparam.push_back(line0->end->y);

				cinfo.constrs[0].cparam.push_back(line1->beg->x);
				cinfo.constrs[0].cparam.push_back(line1->beg->y);
				cinfo.constrs[0].cparam.push_back(line1->end->x);
				cinfo.constrs[0].cparam.push_back(line1->end->y);

                cinfo.objs = uids;

                UID uid = NOUID;
                if ( puid != nullptr ){
                    if (*puid != NOUID )
                        uid = *puid;
                    else
                        *puid = uid = UIDGen::instance()->generate();
                }
                else
                    uid = UIDGen::instance()->generate();

				_constraints[uid] = cinfo;

                _objects[uids[k]].constrs.push_back(uid);
                _objects[uids[k+1]].constrs.push_back(uid);


                MOOLOG << "GeosController::tryAddConstraint - added parallel between line " << *line0 << " and line " << *line1 << " with uid " << uid << std::endl;
            }
            added = true;
        }
        break;
    }
    case CT_VERTICAL:{
    	std::map<ObjectType,std::vector<objInfo*> > grouped;
		groupObj(uids,grouped);
        if ( uids.size() == grouped.size()  && !grouped[OT_SEGMENT].empty() ) {
            for (size_t k= 0; k < grouped[OT_SEGMENT].size(); ++k) {
                lirep* li = dynamic_cast<lirep*>(grouped[OT_SEGMENT][k]->obj);

   				constrInfo cinfo;
   				cinfo.type = CT_VERTICAL;
   				cinfo.constrs.resize(1);
				cinfo.constrs[0].constr = new ConstrEqual(li->beg->x,li->end->x);
                cinfo.constrs[0].cparam.push_back(li->beg->x);
                cinfo.constrs[0].cparam.push_back(li->end->x);

                cinfo.objs = uids;

                UID uid = NOUID;
                if ( puid != nullptr ){
                    if (*puid != NOUID )
                        uid = *puid;
                    else
                        *puid = uid = UIDGen::instance()->generate();
                }
                else
                    uid = UIDGen::instance()->generate();

				_constraints[uid] = cinfo;

                _objects[uids[k]].constrs.push_back(uid);

                MOOLOG << "GeosController::tryAddConstraint - added vertical for line " << *li << " with uid " << uid << std::endl;
            }
            added = true;
        }
        break;
    }
    case CT_HORIZONTAL:{
    	std::map<ObjectType,std::vector<objInfo*> > grouped;
		groupObj(uids,grouped);
        if ( uids.size() == grouped.size() && !grouped[OT_SEGMENT].empty() ) {
            for (size_t k= 0; k < grouped[OT_SEGMENT].size(); ++k) {
                lirep* li = dynamic_cast<lirep*>(grouped[OT_SEGMENT][k]->obj);

   				constrInfo cinfo;
   				cinfo.type = CT_HORIZONTAL;
   				cinfo.constrs.resize(1);
				cinfo.constrs[0].constr = new ConstrEqual(li->beg->y,li->end->y);
                cinfo.constrs[0].cparam.push_back(li->beg->y);
                cinfo.constrs[0].cparam.push_back(li->end->y);

                cinfo.objs = uids;

                UID uid = NOUID;
                if ( puid != nullptr ){
                    if (*puid != NOUID )
                        uid = *puid;
                    else
                        *puid = uid = UIDGen::instance()->generate();
                }
                else
                    uid = UIDGen::instance()->generate();

				_constraints[uid] = cinfo;

				_objects[uids[k]].constrs.push_back(uid);

                MOOLOG << "GeosController::tryAddConstraint - added horizontal for line " << *li << " with uid " << uid << std::endl;
            }
            added = true;
        }
        break;
    }
    case CT_COLLINEAR:{
    	std::map<ObjectType,std::vector<objInfo*> > grouped;
		groupObj(uids,grouped);
        if ( grouped[OT_SEGMENT].size() > 1 ) {
            for (size_t k= 0; k < grouped[OT_SEGMENT].size()-1; ++k) {
				lirep* line0 = dynamic_cast<lirep*>(grouped[OT_SEGMENT][k]->obj);
				lirep* line1 = dynamic_cast<lirep*>(grouped[OT_SEGMENT][k+1]->obj);

				constrInfo cinfo;
				cinfo.type = CT_COLLINEAR;
				cinfo.constrs.resize(2);

				cinfo.constrs[0].constr = new ConstrL2LParal(*line0,*line1);
				cinfo.constrs[0].cparam.push_back(line0->beg->x);
				cinfo.constrs[0].cparam.push_back(line0->beg->y);
				cinfo.constrs[0].cparam.push_back(line0->end->x);
				cinfo.constrs[0].cparam.push_back(line0->end->y);
				cinfo.constrs[0].cparam.push_back(line1->beg->x);
				cinfo.constrs[0].cparam.push_back(line1->beg->y);
				cinfo.constrs[0].cparam.push_back(line1->end->x);
				cinfo.constrs[0].cparam.push_back(line1->end->y);


                _params.push_back(0);
                cinfo.constrs[1].constr = new ConstrP2LDist(*(line0->beg),*line1,&_params.back(),&_params.back());
                cinfo.constrs[1].cparam.push_back(line0->beg->x);
                cinfo.constrs[1].cparam.push_back(line0->beg->y);
                cinfo.constrs[1].cparam.push_back(line1->beg->x);
                cinfo.constrs[1].cparam.push_back(line1->beg->y);
                cinfo.constrs[1].cparam.push_back(line1->end->x);
                cinfo.constrs[1].cparam.push_back(line1->end->y);

                cinfo.objs = uids;

                UID uid = NOUID;
                if ( puid != nullptr ){
                    if (*puid != NOUID )
                        uid = *puid;
                    else
                        *puid = uid = UIDGen::instance()->generate();
                }
                else
                    uid = UIDGen::instance()->generate();

				_constraints[uid] = cinfo;


                MOOLOG << "GeosController::tryAddConstraint - added collinear between line " << *line0 << " and line " << *line1 << " with uid " << uid << std::endl;
            }
            added = true;
        }
        break;
    }
    case CT_TANGENT:{
    	std::map<ObjectType,std::vector<objInfo*> > grouped;
		groupObj(uids,grouped);
        if ( !grouped[OT_SEGMENT].empty() ) {
            if ( grouped[OT_CIRCLE].size() == 1 ){
                circrep* ci = dynamic_cast<circrep*>(grouped[OT_CIRCLE][0]->obj);
                for (size_t k = 0; k< grouped[OT_SEGMENT].size();++k){
                    lirep* li = dynamic_cast<lirep*>(grouped[OT_SEGMENT][k]->obj);

                    constrInfo cinfo;
                    cinfo.type = CT_TANGENT;
                    cinfo.constrs.resize(1);

                    cinfo.constrs[0].constr = new ConstrP2LDist(*(ci->center),*(li),ci->r,true);
                    cinfo.constrs[0].cparam.push_back(ci->center->x);
                    cinfo.constrs[0].cparam.push_back(ci->center->y);
                    cinfo.constrs[0].cparam.push_back(li->beg->x);
                    cinfo.constrs[0].cparam.push_back(li->beg->y);
                    cinfo.constrs[0].cparam.push_back(li->end->x);
                    cinfo.constrs[0].cparam.push_back(li->end->y);
                    cinfo.constrs[0].cparam.push_back(ci->r);

                    cinfo.objs = uids;

                    UID uid = NOUID;
                    if ( puid != nullptr ){
                        if (*puid != NOUID )
                            uid = *puid;
                        else
                            *puid = uid = UIDGen::instance()->generate();
                    }
                    else
                        uid = UIDGen::instance()->generate();

                    _constraints[uid] = cinfo;
                    MOOLOG << "GeosController::tryAddConstraint - added tangent for line " << *li<< " and circle ("<< *(ci->center) <<","<< *(ci->r) << ")"<< " with uid " << uid << std::endl;
                    added = true;
                }
            }
            if ( grouped[OT_ARC].size() == 1 ){
                arcrep* ci = dynamic_cast<arcrep*>(grouped[OT_ARC][0]->obj);
                for (size_t k = 0; k< grouped[OT_SEGMENT].size();++k){
                    lirep* li = dynamic_cast<lirep*>(grouped[OT_SEGMENT][k]->obj);

                    constrInfo cinfo;
                    cinfo.type = CT_TANGENT;
                    cinfo.constrs.resize(1);
/*
                    cinfo.constrs[0].constr = new ConstrPP2LDist(*(arc->center),*(li),ci->r,true);
                    cinfo.constrs[0].cparam.push_back(ci->center->x);
                    cinfo.constrs[0].cparam.push_back(ci->center->y);
                    cinfo.constrs[0].cparam.push_back(li->beg->x);
                    cinfo.constrs[0].cparam.push_back(li->beg->y);
                    cinfo.constrs[0].cparam.push_back(li->end->x);
                    cinfo.constrs[0].cparam.push_back(li->end->y);
                    cinfo.constrs[0].cparam.push_back(ci->r);
*/
                    cinfo.objs = uids;

                    UID uid = NOUID;
                    if ( puid != nullptr ){
                        if (*puid != NOUID )
                            uid = *puid;
                        else
                            *puid = uid = UIDGen::instance()->generate();
                    }
                    else
                        uid = UIDGen::instance()->generate();

                    _constraints[uid] = cinfo;
                    //MOOLOG << "GeosController::tryAddConstraint - added tangent for line " << *li<< " and circle ("<< *(ci->center) <<","<< *(ci->r) << ")"<< " with uid " << uid << std::endl;
                    added = true;
                }

            }
        }
        break;
    }
    case CT_EQUAL:{
       	std::map<ObjectType,std::vector<objInfo*> > grouped;
		groupObj(uids,grouped);

        if ( grouped[OT_CIRCLE].size() >1 ) {
            for ( size_t k= 0; k < grouped[OT_CIRCLE].size()-1; ++k ) {
				circrep* ci0 = dynamic_cast<circrep*>(grouped[OT_CIRCLE][k]->obj);
				circrep* ci1 = dynamic_cast<circrep*>(grouped[OT_CIRCLE][k+1]->obj);

				constrInfo cinfo;
				cinfo.type = CT_EQUAL;
				cinfo.constrs.resize(1);

				cinfo.constrs[0].constr = new ConstrEqual(ci0->r,ci1->r);
				cinfo.constrs[0].cparam.push_back(ci0->r);
				cinfo.constrs[0].cparam.push_back(ci1->r);

                cinfo.objs = uids;

                UID uid = NOUID;
                if ( puid != nullptr ){
                    if (*puid != NOUID )
                        uid = *puid;
                    else
                        *puid = uid = UIDGen::instance()->generate();
                }
                else
                    uid = UIDGen::instance()->generate();

				_constraints[uid] = cinfo;

                MOOLOG << "GeosController::tryAddConstraint - added equal radius for circles " << *ci0->center << " and " << *ci1->center << std::endl;
            }

            added = true;
        }
        if ( grouped[OT_SEGMENT].size() > 1 ) {
            for (size_t k= 0; k < grouped[OT_SEGMENT].size()-1; ++k) {
				lirep* line0 = dynamic_cast<lirep*>(grouped[OT_SEGMENT][k]->obj);
				lirep* line1 = dynamic_cast<lirep*>(grouped[OT_SEGMENT][k+1]->obj);

				constrInfo cinfo;
				cinfo.type = CT_EQUAL;
				cinfo.constrs.resize(1);

				cinfo.constrs[0].constr = new ConstrL2LEqual(*line0,*line1);
				cinfo.constrs[0].cparam.push_back(line0->beg->x);
				cinfo.constrs[0].cparam.push_back(line0->beg->y);
				cinfo.constrs[0].cparam.push_back(line0->end->x);
				cinfo.constrs[0].cparam.push_back(line0->end->y);

				cinfo.constrs[0].cparam.push_back(line1->beg->x);
				cinfo.constrs[0].cparam.push_back(line1->beg->y);
				cinfo.constrs[0].cparam.push_back(line1->end->x);
				cinfo.constrs[0].cparam.push_back(line1->end->y);

                cinfo.objs = uids;

                UID uid = NOUID;
                if ( puid != nullptr ){
                    if (*puid != NOUID )
                        uid = *puid;
                    else
                        *puid = uid = UIDGen::instance()->generate();
                }
                else
                    uid = UIDGen::instance()->generate();


				_constraints[uid] = cinfo;

                MOOLOG << "GeosController::tryAddConstraint - added equal for lines " << *line0 << " and " << *line1 << std::endl;

            }
            added = true;
        }
        if ( grouped[OT_POINT].size() == 4 ) {
            for (size_t k= 0; k < grouped[OT_POINT].size()-1; k+=4) {
                ptrep* pb1 = dynamic_cast<ptrep*>(grouped[OT_POINT][k]->obj);
                ptrep* pe1 = dynamic_cast<ptrep*>(grouped[OT_POINT][k+1]->obj);
                ptrep* pb2 = dynamic_cast<ptrep*>(grouped[OT_POINT][k+2]->obj);
                ptrep* pe2 = dynamic_cast<ptrep*>(grouped[OT_POINT][k+3]->obj);

                constrInfo cinfo;
                cinfo.type = CT_EQUAL;
                cinfo.constrs.resize(1);

                cinfo.constrs[0].constr = new ConstrL2LEqual(*pb1,*pe1,*pb2,*pe2);

                cinfo.constrs[0].cparam.push_back(pb1->x);
                cinfo.constrs[0].cparam.push_back(pb1->y);
                cinfo.constrs[0].cparam.push_back(pe1->x);
                cinfo.constrs[0].cparam.push_back(pe1->y);

                cinfo.constrs[0].cparam.push_back(pb2->x);
                cinfo.constrs[0].cparam.push_back(pb2->y);
                cinfo.constrs[0].cparam.push_back(pe2->x);
                cinfo.constrs[0].cparam.push_back(pe2->y);

                cinfo.objs = uids;

                UID uid = NOUID;
                if ( puid != nullptr ){
                    if (*puid != NOUID )
                        uid = *puid;
                    else
                        *puid = uid = UIDGen::instance()->generate();
                }
                else
                    uid = UIDGen::instance()->generate();


                _constraints[uid] = cinfo;

                MOOLOG << "GeosController::tryAddConstraint - added equal for points " << *pb1 << " and " << *pe1 << " and " << *pb2 << " and " << *pe2 << std::endl;

            }
            added = true;
        }
        break;        
    }
    case CT_CONCENTRIC:{
        std::map<ObjectType,std::vector<objInfo*> > grouped;
        groupObj(uids,grouped);

        if ( grouped[OT_CIRCLE].size() > 1 ) {
            for ( size_t k= 0; k < grouped[OT_CIRCLE].size()-1; ++k ) {
                circrep* ci0 = dynamic_cast<circrep*>(grouped[OT_CIRCLE][k]->obj);
                circrep* ci1 = dynamic_cast<circrep*>(grouped[OT_CIRCLE][k+1]->obj);


                constrInfo cinfo;
                cinfo.type = CT_CONCENTRIC;
                cinfo.constrs.resize(2);

                cinfo.constrs[0].constr = new ConstrEqual(ci0->center->x,ci1->center->x);
                cinfo.constrs[0].cparam.push_back(ci0->center->x);
                cinfo.constrs[0].cparam.push_back(ci1->center->x);

                cinfo.constrs[1].constr = new ConstrEqual(ci0->center->y,ci1->center->y);
                cinfo.constrs[1].cparam.push_back(ci0->center->y);
                cinfo.constrs[1].cparam.push_back(ci1->center->y);

                cinfo.objs = uids;

                UID uid = NOUID;
                if ( puid != nullptr ){
                    if (*puid != NOUID )
                        uid = *puid;
                    else
                        *puid = uid = UIDGen::instance()->generate();
                }
                else
                    uid = UIDGen::instance()->generate();

                _constraints[uid] = cinfo;

                _objects[uids[0]].constrs.push_back(uid);
                _objects[uids[1]].constrs.push_back(uid);

                MOOLOG << "GeosController::tryAddConstraint - added concentric for circles " << *ci0->center << " and " << *ci1->center << std::endl;
            }

            added = true;
        }
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
		auto it = _objects.find(uids[k]);
		if ( it == _objects.end() ) return RC_NO_OBJ;
		size_t c = 0;
		while (c < objInfo::MAXCHILDS && (*it).second.objChilds[c] != NOUID){
			for (size_t k1 = 0;k1< uids.size();++k1)
				if (uids[k] == (*it).second.objChilds[c]) return -1;
			++c;
		}
	}

    MOOLOG << "georis::Core::addConstraint " << uids.size() << " objects" << std::endl;

	backupState();
    RESCODE res = tryAddConstraint(type,uids,param,puid);
    MOOLOG << "georis::Core::addConstraint " << res << " tac" << std::endl;
    if ( RC_OK != res ) return res;
	res = solve();
    if ( RC_OK == res ) return res;
	restoreState();
	return res;
}
RESCODE georis::Core::removeConstraint(UID id2rem) {
	std::map<UID,constrInfo >::iterator cit = _constraints.find(id2rem);
	if ( cit == _constraints.end() ) return RC_NO_OBJ;
	// clear constraint from linked objects
	for (size_t k = 0;k < (*cit).second.objs.size();++k)
		for (size_t m = 0;m < _objects[(*cit).second.objs[k]].constrs.size();++m )
			if ( _objects[(*cit).second.objs[k]].constrs[m] == id2rem ){
				_objects[(*cit).second.objs[k]].constrs.erase(_objects[(*cit).second.objs[k]].constrs.begin()+m);
				break;
			}

	// clear _constraints
	_constraints.erase(cit);
	return RC_OK;
}
RESCODE georis::Core::queryConstrInfo(UID uid,ConstraintType &type,std::vector<UID> &objs,double *pparam)const {
	auto it = _constraints.find(uid);
	if (it == _constraints.end()) return RC_NO_OBJ;

	type = (*it).second.type;
	objs.clear();
    objs = (*it).second.objs;
    return RC_OK;
}
void georis::Core::enumConstraints(std::vector<UID>&uids)const{
	uids.clear();
	uids.reserve(_constraints.size());
    for (auto it : _constraints)
        uids.push_back(it.first);
}
void georis::Core::groupObj(const std::vector<UID> &uids, std::map<ObjectType,std::vector<objInfo*> > &grouped){
	grouped.clear();
	for (size_t k = 0;k<uids.size();++k){
		auto objit = _objects.find(uids[k]);
		if ( objit == _objects.end() ) continue;
		grouped[(*objit).second.obj->type()].push_back(&((*objit).second));
	}
	for (auto it = grouped.begin();it != grouped.end();++it)
        MOOLOG << "georis::Core::groupObj type = " << (*it).first << " count " << (*it).second.size() << std::endl;
}
int georis::Core::solve(){
	auto t1 = Clock::now();
	if ( !_constraints.empty() ) {
        MOOLOG << "georis::Core::solve - there are "<< _constraints.size() << " constraints" << std::endl;

        // collect constraints for parameters and parameters for constraints
        std::map<double *, std::vector<IConstraint *> > p2c;
        std::map<IConstraint *,std::vector<double *> > c2p;
        for (auto cit = _constraints.begin(); cit != _constraints.end(); ++cit) {
            MOOLOG << "georis::Core::solve -  (*cit).second.constrs.size() = " << (*cit).second.constrs.size() << std::endl;
			for (size_t nc = 0;nc < (*cit).second.constrs.size();++nc){
                //MOOLOG << "georis::Core::solve -  (*cit).second.constrs[nc].cparam.size() = " << (*cit).second.constrs[nc].cparam.size() << std::endl;
				for (auto pit = (*cit).second.constrs[nc].cparam.begin(); pit != (*cit).second.constrs[nc].cparam.end(); ++pit){
					p2c[*pit].push_back( (*cit).second.constrs[nc].constr);
                    //MOOLOG << *pit << std::endl;
				}
				c2p[ (*cit).second.constrs[nc].constr ] = (*cit).second.constrs[nc].cparam;
			}
        }
//        MOOLOG << "georis::Core::solve - p2c.size() = " << p2c.size() << std::endl;
        //MOOLOG << "georis::Core::solve - c2p.size() = " << c2p.size() << std::endl;
        //MOOLOG << "georis::Core::solve - c2p[0].size() = " << (*c2p.begin()).second.size() << std::endl;

        // extract constraints groups
        std::list<std::list<IConstraint*> > constrGroups;
        std::stack<IConstraint*> cStack;
		std::map<IConstraint *,std::vector<double *> > c2pbu = c2p;
        while ( !c2pbu.empty() ) {
            constrGroups.push_back( std::list<IConstraint*>() );
            constrGroups.back().push_back( (*c2pbu.begin()).first );
            std::vector<double *> pars;
            pars.swap((*c2pbu.begin()).second);
            c2pbu.erase(c2pbu.begin());
            for (unsigned p = 0; p < pars.size(); ++p)
                for (unsigned c = 0; c < p2c[pars[p]].size(); ++c)
                    cStack.push(p2c[pars[p]][c]);

            while (!cStack.empty()) {
                auto cit = c2pbu.find(cStack.top());
                if ( cit != c2pbu.end() ) {
                    std::vector<double *> pars = (*cit).second;
                    constrGroups.back().push_back((*cit).first);
                    cStack.pop();
                    for (unsigned p = 0; p<pars.size(); ++p)
                        for (unsigned c = 0; c < p2c[pars[p]].size(); ++c)
                            cStack.push(p2c[pars[p]][c]);
                    c2pbu.erase(cit);
                } else
                    cStack.pop();
            }

        }

        MOOLOG << "georis::Core::solve - constrGroups.size() = " << constrGroups.size() << std::endl;



        for (auto cgi = constrGroups.begin(); cgi != constrGroups.end(); ++cgi) {
            MOOLOG << "georis::Core::solve group size " << (*cgi).size() << std::endl;
            std::set<double*> param_sorter;
            for (auto ci = (*cgi).begin(); ci != (*cgi).end(); ++ci) {
                auto mi = c2p.find(*ci);
                if ( mi != c2p.end() ){
                    MOOLOG << "georis::Core::solve param size" << (*mi).second.size() << std::endl;
                    for (auto vi = (*mi).second.begin(); vi != (*mi).second.end(); ++vi)
                        param_sorter.insert(*vi);
                }
            }
            // Remove fixed parameters

            for (auto it = _objects.begin(); it != _objects.end(); ++it){
				if ( (*it).second.obj->fixed ) {
					switch ((*it).second.obj->type()){
					case OT_POINT:{
						ptrep *pt = dynamic_cast<ptrep*>((*it).second.obj);
						param_sorter.erase( pt->x) ;
						param_sorter.erase( pt->y) ;
						break;
					}
					case OT_SEGMENT:{
						lirep *li = dynamic_cast<lirep*>((*it).second.obj);
						param_sorter.erase( li->beg->x) ;
						param_sorter.erase( li->beg->y);
						param_sorter.erase( li->end->x) ;
						param_sorter.erase( li->end->y);
						break;
					}
					case OT_CIRCLE:{
						circrep *ci = dynamic_cast<circrep*>((*it).second.obj);
						param_sorter.erase( ci->center->x);
						param_sorter.erase( ci->center->y);
						param_sorter.erase( ci->r);
						break;
					}
					default:
						;
					}
                }
            }
            MOOLOG << "georis::Core::solve - there are "<< param_sorter.size() << " tunable params" << std::endl;
            std::vector<double *> params;
            params.reserve(param_sorter.size());
            for (auto it = param_sorter.begin(); it != param_sorter.end(); ++it)
                params.push_back(*it);

            // Create function for constraints group
            GeosFuncN func(&params);
            for (auto it = (*cgi).begin(); it != (*cgi).end(); ++it)
                func.add(*it);
            SolveTask geotask;
            geotask.target = &func;

            geotask.x0 = v_type(params.size());
            for (size_t k = 0; k<params.size(); ++k)
                geotask.x0(k) = *params[k];
            v_type err = func(geotask.x0);
            MOOLOG << "GeosController::solve - initial error = " << err << ", norm = " << (err.transpose()*err) << std::endl;
            MOOLOG << "GeosController::solve - at " << std::endl << geotask.x0 << std::endl;
            geotask.stopcond.tolx = 1e-15;
            geotask.stopcond.tolf = 1e-15;
            geotask.stopcond.fevals = 10000;

            //SolverCeres solver;
            SolverLM solver;
            //SolverNG solver;

            solver.solve(geotask);

            for (size_t k = 0; k<params.size(); ++k)
                geotask.x0(k) = *params[k];
            err = func(geotask.x0);
//            MOOLOG << "GeosController::solve - best value so far = " << (err.transpose()*err) << std::endl;
            //MOOLOG << "GeosController::solve - at " << std::endl << geotask.x0 << std::endl;
            //MOOLOG << "=============================" << std::endl;

        }
    }
    //MOOLOG << "=============================" << std::endl;
	auto t2 = Clock::now();
    MOOLOG << "georis::Core::solve Delta t2-t1: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " ms" << std::endl;
	return 0;
}
RESCODE georis::Core::moveObjects(const std::vector<UID>& objs,double dx, double dy){
	auto objsnochi(objs);
    filterChildObj(objsnochi);
    MOOLOG << "georis::Core::moveObjects moving " << objsnochi.size() << std::endl;
	backupState();
    for (auto v:objsnochi){
		auto it = _objects.find(v);
		if ( it != _objects.end() )
			(*it).second.obj->move(dx,dy);
    }
    int res = solve();
	if ( 0 == res ) return res;
    MOOLOG << "georis::Core::moveObjects infeasible move, restoring " << std::endl;
	restoreState();
	return res;
}
void georis::Core::filterChildObj(std::vector<UID> &objs)const{
	size_t k = 0;
	while (k < objs.size() ){
		UID uidpar = NOUID;
		for ( auto objit = _objects.begin();objit != _objects.end(); ++ objit)
			if ( (*objit).second.hasChild(objs[k]) ) {
				uidpar = (*objit).first;
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

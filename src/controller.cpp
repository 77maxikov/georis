#include <set>
#include <stack>
#include <iostream>
#include <fstream>
#include <limits>
#include <algorithm>
#include <cassert>

#include "controller.h"
#include "iosvgx.h"

#include "mooLog.h"

//#include "geoobj_utils.h"


const double georis::Controller::_sel_precision = 0.1;

georis::Controller::Controller() {
    m_uidActiveSketch = NOUID;
}

georis::Controller::~Controller() {

}
void georis::Controller::createNew(){
    /*
    if ( m_uidActiveSketch != NOUID && m_bIsModified ){
        // save current

        // remove previous
        m_core.removeSketch(m_uidActiveSketch);
    }
    m_core.createSketch(&m_uidActiveSketch);
    */
    m_core = Core();
    m_ui->reset();
}

void georis::Controller::setUI(IVisualizer *vis) {
    if ( vis != nullptr ) m_ui = vis;
    showSelectionInfo();
}
void georis::Controller::updateView() {
    //MOOLOG << "GeosController::updateView: " << m_objs.size() << " objects to update" << std::endl;
    for (auto it: m_objs){
        ObjectType objtype;
        std::vector<double> param;
        RESCODE res = m_core.queryObjInfo( it.first,objtype,param );
        if ( res != RC_OK ){
            MOOLOG << "Controller::updateView no such obj with uid " << it.first << std::endl;
            return;
        }

        double *para = &*param.begin();
        switch (objtype){
        case ObjectType::OT_POINT:
            m_ui->drawPoint(para,para+1,it.second.status/*|(it.second?MODE_FIXED:MODE_NORMAL)*/);
            break;
        case ObjectType::OT_SEGMENT:
            m_ui->drawLine(para,para+1,para+2,para+3,it.second.status/*|(info.status?MODE_FIXED:MODE_NORMAL)*/);
            break;
        case ObjectType::OT_CIRCLE:
            m_ui->drawCircle(para,para+1,para+2,it.second.status/*|(info.status?MODE_FIXED:MODE_NORMAL)*/);
            break;
        default:
            ;
        }
    }
}
void georis::Controller::addObject(georis::ObjectType type, const std::vector<double> &param,const std::string &name){
    MOOLOG << "Controller::addObject " << type;
    if ( !param.empty() ) {
        MOOLOG << " with parameters";
        for (size_t k = 0; k< param.size(); ++k)
            MOOLOG << ' ' << param[k];
        MOOLOG << std::endl;
    }
    UID uid = NOUID;
    RESCODE res = m_core.addObject(type,param,&uid);
    if ( res != RC_OK ){
        MOOLOG << "Controller::addObject unsuccesful" << std::endl;
        return;
    }
    MOOLOG << "Controller::addObject added parent with UID " << uid << std::endl;

    std::vector<UID> ids;
    m_core.getObjChilds(uid,ids);


    const size_t bufsize = 64;
    static char buf[bufsize];

    if (name.empty()) { // Create object's name
        _lastObjNums[type]++;
        switch(type) {
        case OT_POINT:
            snprintf(buf,bufsize,"Точка%d",_lastObjNums[type]);
            break;
        case OT_SEGMENT:
            snprintf(buf,bufsize,"Отрезок%d",_lastObjNums[type]);
            break;
        case OT_CIRCLE:
            snprintf(buf,bufsize,"Окружность%d",_lastObjNums[type]);
            break;
        case OT_ARC:
            snprintf(buf,bufsize,"Дуга%d",_lastObjNums[type]);
            break;
        default:
            ;
        }
        m_objs[uid].name = buf;
    }
    else
        m_objs[uid].name = name;
    m_objs[uid].status = 0;

    std::vector<UID> chids;
    res = m_core.getObjChilds(uid,chids);
    if (res  != RC_OK ){
        MOOLOG << "Controller::addObject can't get new object children" << std::endl;
        return ;
    }
    for (auto chuid: chids){
        ObjectType ot;
        res = m_core.getObjType(chuid,ot);

        assert( res == RC_OK );

        if ( ot == OT_POINT ){
            _lastObjNums[OT_POINT]++;
            snprintf(buf,bufsize,"Точка%d",_lastObjNums[OT_POINT]);
        }
        m_objs[chuid].status = 0;
        m_objs[chuid].name = buf;
        MOOLOG << "Controller::addObject added child with UID " << chuid << std::endl;
    }
    // Add implicit constraints

    if ( m_memHighlights[0] != NOUID ){
        switch(type){
        case OT_POINT:{
            std::vector<UID> cobjs;
            cobjs.push_back(uid);
            cobjs.push_back(m_memHighlights[0]);
            addConstraint(CT_COINCIDENT,cobjs);
            break;
        }
        case OT_SEGMENT:{
            std::vector<UID> cobjs;
            cobjs.push_back(ids[0]);
            cobjs.push_back(m_memHighlights[0]);
            addConstraint(CT_COINCIDENT,cobjs);
            break;
        }
        case OT_CIRCLE:{
            std::vector<UID> cobjs;
            cobjs.push_back(ids[0]);
            cobjs.push_back(m_memHighlights[0]);
            addConstraint(CT_COINCIDENT,cobjs);
            break;
        }
        default:
            ;
        }
    }

    if ( m_memHighlights[1] != NOUID ){
        switch(type){
        case OT_SEGMENT:{
            std::vector<UID> cobjs;
            cobjs.push_back(ids[1]);
            cobjs.push_back(m_memHighlights[1]);
            addConstraint(CT_COINCIDENT,cobjs);
            break;
        }
        case OT_CIRCLE:{
            std::vector<UID> cobjs;
            cobjs.push_back(ids[1]);
            cobjs.push_back(m_memHighlights[1]);
            addConstraint(CT_COINCIDENT,cobjs);
            break;
        }
        default:
            ;
        }
    }

    m_memHighlights[0] = NOUID;
    m_memHighlights[1] = NOUID;
}

void georis::Controller::addConstraint(ConstraintType type, const std::vector<UID> &objects, double param, const std::string &name){
    UID uid = NOUID;
    RESCODE res = m_core.addConstraint(type,objects,param,&uid);
    if ( res != RC_OK ){
        MOOLOG << "Controller::addConstraint unsuccesful" << std::endl;
        return;
    }
    MOOLOG << "Controller::addConstraint added with UID " << uid << std::endl;

    const size_t bufsize = 64;
    static char buf[bufsize];

    if (name.empty()) { // Create object's name
        _lastConstrNums[type]++;
        switch(type) {
        case CT_FIX:
            snprintf(buf,bufsize,"Зафиксировано%d",_lastConstrNums[type]);
            break;
        case CT_EQUAL:
            snprintf(buf,bufsize,"Равно%d",_lastConstrNums[type]);
            break;
        case CT_VERTICAL:
            snprintf(buf,bufsize,"Вертикально%d",_lastConstrNums[type]);
            break;
        case CT_HORIZONTAL:
            snprintf(buf,bufsize,"Горизонтально%d",_lastConstrNums[type]);
            break;
        case CT_DISTANCE:
            snprintf(buf,bufsize,"Расстояние%d",_lastConstrNums[type]);
            break;
        case CT_ANGLE:
            snprintf(buf,bufsize,"Угол%d",_lastConstrNums[type]);
            break;
        case CT_PARALLEL:
            snprintf(buf,bufsize,"Параллельно%d",_lastConstrNums[type]);
            break;
        case CT_ORTHO:
            snprintf(buf,bufsize,"Перпендикулярно%d",_lastConstrNums[type]);
            break;
        case CT_TANGENT:
            snprintf(buf,bufsize,"Касательно%d",_lastConstrNums[type]);
            break;
        case CT_COINCIDENT:
            snprintf(buf,bufsize,"Принадлежит%d",_lastConstrNums[type]);
            break;
        case CT_MIDPOINT:
            snprintf(buf,bufsize,"Середина%d",_lastConstrNums[type]);
            break;
        case CT_COLLINEAR:
            snprintf(buf,bufsize,"Колинеарно%d",_lastConstrNums[type]);
            break;
        case CT_DIMENSION:
            snprintf(buf,bufsize,"Размер%d",_lastConstrNums[type]);
            break;
        case CT_SYMMETRIC:
            snprintf(buf,bufsize,"Симметрично%d",_lastConstrNums[type]);
            break;
        case CT_CONCENTRIC:
            snprintf(buf,bufsize,"Концентрично%d",_lastConstrNums[type]);
        }

        m_constrs[uid].name = buf;
    }
    else
        m_constrs[uid].name = name;
    m_constrs[uid].status = 0;

}

void georis::Controller::resetSelection() {
    MOOLOG << "Controller::resetSelection" << std::endl;
    for (auto && it : m_objs ){
        if ( it.second.status & MODE_SELECTED ){
            it.second.status &= ~MODE_SELECTED;
            MOOLOG << "Controller::resetSelection reset for " << it.first << std::endl;
        }
    }
    showSelectionInfo();
}
void georis::Controller::selectByPoint(double x,double y,double precision) {
    point2r p(&x,&y);
    std::vector<UID> nearest;
    std::vector<double> dists;
    m_core.findObjInCirc(p,precision,nearest,&dists);

    if ( !nearest.empty() ){
        std::map<ObjectType,std::map<double,UID> >	 sorter;
        for ( size_t k = 0 ; k < nearest.size();++k ){

            ObjectType ot;
            std::vector<double> param;
            m_core.queryObjInfo(nearest[k],ot, param);
            sorter[ot][dists[k]] = nearest[k];

        }

        if ( sorter.find(OT_POINT) != sorter.end() ){
            m_objs[ (*sorter[OT_POINT].begin()).second ].status ^= MODE_SELECTED;
            MOOLOG << "Controller::selectByPoint size " << m_objs.size() << std::endl;
            showSelectionInfo();
            return;
        }
        double bestdist = std::numeric_limits<double>::infinity();
        ObjectType bestt = OT_NONE;
        for ( auto it  = sorter.begin(); it != sorter.end();++it ){
            double di = (*((*it).second.begin())).first;
            if ( di < bestdist ){
                bestdist = di;
                bestt = (*it).first;
            }
        }
        UID sel = (*sorter[bestt].begin()).second;

        UID par;
        m_core.getObjParent(sel,par);
        if ( par != NOUID  &&  (m_objs[par].status & MODE_SELECTED) && (m_objs[sel].status & MODE_SELECTED) ) return;

        m_objs[sel].status ^= MODE_SELECTED;

        unsigned status = m_objs[sel].status;

        std::vector<UID> subs;
        m_core.getObjChilds(sel,subs);
        if ( !subs.empty() )
            for ( size_t s = 0 ; s < subs.size();++s )
                m_objs[subs[s]].status ^= (m_objs[subs[s]].status ^ status) & MODE_SELECTED;
        showSelectionInfo();
    }
    else
        resetSelection();

    MOOLOG << "Controller::selectByPoint L size " << m_objs.size() << std::endl;
}

void georis::Controller::selectByRect(double x1,double y1,double x2,double y2) {
    double xmin = std::min(x1,x2);
    double xmax = std::max(x1,x2);
    double ymin = std::min(y1,y2);
    double ymax = std::max(y1,y2);

    point2r p1(&xmin,&ymin);
    point2r p2(&xmax,&ymax);
    std::vector<UID> nearest;
    m_core.findObjInRect(p1,p2,nearest);

    if ( !nearest.empty() ){
        for ( size_t k = 0 ; k < nearest.size();++k ){
            UID par;
            m_core.getObjParent(nearest[k],par);
            if ( par != NOUID  &&  m_objs[par].status & MODE_SELECTED && m_objs[nearest[k]].status & MODE_SELECTED) continue;

            m_objs[nearest[k]].status ^= MODE_SELECTED;

            unsigned status = m_objs[nearest[k]].status;

            std::vector<UID> subs;
            m_core.getObjChilds(nearest[k],subs);
            for ( size_t s = 0 ; s < subs.size();++s )
                m_objs[subs[s]].status ^= (m_objs[subs[s]].status ^ status) & MODE_SELECTED;

        }
        showSelectionInfo();
    }
}

void georis::Controller::resetHighlight() {
    for (auto it = m_objs.begin(); it != m_objs.end(); ++it){
        if ( (*it).second.status & MODE_HIGHLIGHTED )
            (*it).second.status &= ~MODE_HIGHLIGHTED;
    }
}
void georis::Controller::highlightObj(double x,double y,double precision) {
    resetHighlight();
    point2r p(&x,&y);
    std::vector<UID> nearest;
    std::vector<double> dists;
    m_core.findObjInCirc(p,precision,nearest,&dists);

    if ( !nearest.empty() ){
        std::map<ObjectType,std::map<double,UID> >	 sorter;
        for ( size_t k = 0 ; k < nearest.size();++k ){
            ObjectType ot;
            m_core.getObjType(nearest[k],ot);
            sorter[ot][dists[k]] = nearest[k];
        }
        MOOLOG << "Controller::highlightObj sorter size " << sorter.size() << std::endl;
        if ( sorter.find(OT_POINT) != sorter.end() ){
            m_objs[ (*sorter[OT_POINT].begin()).second ].status |= MODE_HIGHLIGHTED;
            MOOLOG << "Controller::highlightObj _objs.size " << m_objs.size() << std::endl;
            return;
        }
        double bestdist = std::numeric_limits<double>::infinity();
        ObjectType bestt = OT_NONE;
        for ( auto it  = sorter.begin(); it != sorter.end();++it ){
            double di = (*((*it).second.begin())).first;
            MOOLOG << "Controller::highlightObj ot " << (*it).first << " di " << di << std::endl;
            if ( di < bestdist ){
                bestdist = di;
                bestt = (*it).first;
            }
        }

        m_objs[(*sorter[bestt].begin()).second].status |= MODE_HIGHLIGHTED;
        MOOLOG << "Controller::highlightObj bestt " << bestt << " bestdist " << bestdist << " _objs.size() " << m_objs.size() << std::endl;
    }
}
void georis::Controller::constrainSelected(ConstraintType type,double param) {
    MOOLOG << "Controller::addConstraint constraint " << type << " with parameter " << param << std::endl;
    std::vector<UID> selected;
    findObj(MODE_SELECTED, selected);
    std::vector<UID> filtered;
    filtered.reserve(selected.size());
    std::vector<UID> subob;
    for (size_t k = 0;k<selected.size();++k){
        std::vector<UID> sub;
        m_core.getObjChilds(selected[k],sub);
        if (!sub.empty())
            subob.insert(subob.end(),sub.begin(),sub.end());
    }
    std::set_difference(selected.begin(),selected.end(),subob.begin(),subob.end(),std::inserter(filtered,filtered.begin()));
    addConstraint(type,filtered,param);
    resetSelection();
    showSelectionInfo();
}
void georis::Controller::deleteSelected() {
    std::vector<UID> selected;
    findObj(MODE_SELECTED, selected);
    for (size_t k = 0; k < selected.size(); ++k){
        UID uidpar;
        m_core.getObjParent(selected[k],uidpar);
        if ( uidpar != NOUID ) continue;
        std::vector<UID> subs;
        m_core.getObjChilds(selected[k],subs);
        m_core.removeObject(selected[k]);

        for ( size_t s = 0 ; s < subs.size();++s )
            m_objs.erase(subs[s]);
        m_objs.erase(selected[k]);
    }
    showSelectionInfo();
}
void georis::Controller::findObj(unsigned stateMask,std::vector<UID> &res){
    res.clear();
    for (auto it : m_objs )
        if ( it.second.status & stateMask )
            res.push_back(it.first);
}

void georis::Controller::moveSelected(double dx,double dy) {
    std::vector<UID> selected;
    findObj(MODE_SELECTED,selected);
    if ( !selected.empty() )
        m_core.moveObjects(selected,dx,dy);
}
void georis::Controller::showSelectionInfo() {
    if ( m_ui == nullptr ) return;
    // Collect selected objects info

    int nselPoints = 0, nselLines = 0, nselCircls = 0;
    std::vector<UID> selected;
    findObj(MODE_SELECTED,selected);
    //if ( selected.empty() ) return;
    m_core.filterChildObj(selected);

    std::map<UID,std::string> namedObjs;
    for (auto it : selected){
        ObjectType ot;
        m_core.getObjType(it,ot);
        switch ( ot ){
        case OT_POINT:
            ++nselPoints;
            break;
        case OT_SEGMENT:
            ++nselLines;
            //nselPoints-=2;
            break;
        case OT_CIRCLE:
            ++nselCircls;
            //--nselPoints;
        case OT_ARC:
        case OT_SPLINE:
        case OT_NONE:
            ;

        }
        namedObjs[it] = m_objs[it].name;
    }

    MOOLOG << "Controller::showSelectionInfo: total selected "<< selected.size() << std::endl;
    MOOLOG << "                                          nPoints = "<< nselPoints << " nLines = " << nselLines << " nCircls = " << nselCircls << std::endl;

    m_ui->setSelectedObjs(namedObjs);

    // Collect common constrains for selected objects
    std::vector<UID> commonSelConstrs;
    for (auto it = selected.begin(); it != selected.end();++it){
        std::vector<UID> tmp;
        m_core.getObjConstraints(*it,tmp);
        if ( commonSelConstrs.empty() ){
            commonSelConstrs.swap(tmp);
            continue;
        }
        std::vector<UID> base;
        base.swap(commonSelConstrs);
        std::set_intersection(base.begin(),base.end(),tmp.begin(),tmp.end(),std::back_inserter(commonSelConstrs));
    }
    std::map<UID,std::string> namedConstrs;
    for (auto it : commonSelConstrs )
        namedConstrs[it] = m_constrs[it].name;

    m_ui->setSelectedConstraints(namedConstrs);

    // Show available constraints
    std::vector<ConstraintType>  constr;
    if ( nselPoints || nselLines || nselCircls ) {
        constr.push_back(CT_FIX);
        if ( nselPoints > 1 && nselLines == 0  && nselCircls == 0 ) // Only points selected
            constr.push_back(CT_COINCIDENT);

        if ( nselPoints == 0 && nselLines > 0 && nselCircls == 0 ) { // Only lines are selected
            constr.push_back(CT_VERTICAL);
            constr.push_back(CT_HORIZONTAL);
            if ( nselLines == 1)
                constr.push_back(CT_DIMENSION);
            else {
                constr.push_back(CT_PARALLEL);
                constr.push_back(CT_COLLINEAR);
                constr.push_back(CT_EQUAL);
                if ( nselLines == 2 ){
                    constr.push_back(CT_ANGLE);
                    constr.push_back(CT_ORTHO);
                }
            }
        }
        if ( nselPoints == 0 && nselLines == 0 ) { // Only circles selected
            if ( nselCircls == 1 )
                constr.push_back(CT_DIMENSION);
            else{
                constr.push_back(CT_EQUAL);
                constr.push_back(CT_CONCENTRIC);
                constr.push_back(CT_TANGENT);
            }
        }
        if ( nselPoints == 1 ){
            if ( nselLines == 1 && nselCircls == 0 ){
                constr.push_back(CT_COINCIDENT);
                constr.push_back(CT_MIDPOINT);
            }
            if ( nselCircls == 1 && nselLines == 0) {
                constr.push_back(CT_COINCIDENT);
            }
        }
        if ( nselPoints == 2 && nselLines == 1 && nselCircls == 0 )
            constr.push_back(CT_SYMMETRIC);
        if ( nselPoints == 0 && nselLines == 1 && nselCircls == 1 )
            constr.push_back(CT_TANGENT);

    }
    m_ui->setAvailConstraints(constr);
}
void georis::Controller::memHighlightsDown(){
    m_memHighlights[0] = NOUID;
    std::vector<UID> highlighted;
    findObj(MODE_HIGHLIGHTED,highlighted);
    if ( !highlighted.empty() )
        m_memHighlights[0] = highlighted[0];
    MOOLOG << "GeosController::memHighlightsDown " << m_memHighlights[0] << std::endl;
}
void georis::Controller::memHighlightsUp(){
    m_memHighlights[1] = NOUID;
    std::vector<UID> highlighted;
    findObj(MODE_HIGHLIGHTED,highlighted);
    if ( !highlighted.empty() )
        m_memHighlights[1] = highlighted[0];

    MOOLOG << "GeosController::memHighlightsUp " << m_memHighlights[1] << std::endl;

}
void georis::Controller::saveTo(const std::string &fname){
    std::vector<UID> objuids;
    m_core.enumObjs(objuids);
    m_core.filterChildObj(objuids);


    SVGXWriter writer;
    RESCODE res = writer.prepare(fname.c_str());
    if ( res != RC_OK ){
        MOOLOG << "Controller::saveTo: can't prepare file " << fname << std::endl;
        return;
    }

    for (auto uid:objuids){
        std::vector<UID> chuids;
        m_core.getObjChilds(uid,chuids);
        ObjectType ot;
        std::vector<double> param;
        for (auto chuid:chuids){
            res = m_core.queryObjInfo(chuid,ot,param);
            if ( res != RC_OK ){
                MOOLOG << "Controller::saveTo: no obj uid " << chuid << std::endl;
                return;
            }
            res = writer.saveObject(chuid,m_objs[chuid].name,ot,param,uid);
            if ( res != RC_OK ){
                MOOLOG << "Controller::saveTo: can't save obj with uid " << chuid << std::endl;
                return;
            }
        }
        res = m_core.queryObjInfo(uid,ot,param);
        if ( res != RC_OK ){
            MOOLOG << "Controller::saveTo: no obj uid " << uid << std::endl;
            return;
        }
        res = writer.saveObject(uid,m_objs[uid].name,ot,param);
        if ( res != RC_OK ){
            MOOLOG << "Controller::saveTo: can't save obj with uid " << uid << std::endl;
            return;
        }
    }

    std::vector<UID> construids;
    m_core.enumConstraints(construids);
    for (auto construid:construids){
        double para = 0;
        ConstraintType ct;
        res = m_core.queryConstrInfo(construid,ct,objuids,&para);
        if ( res != RC_OK ){
            MOOLOG << "Controller::saveTo:  no constr uid " << construid << std::endl;
            return;
        }
        switch (ct){
        case CT_ANGLE:
        case CT_DISTANCE:
        case CT_DIMENSION:
            writer.saveConstraint(construid,m_constrs[construid].name,ct,objuids,&para);
            break;
        default:
            writer.saveConstraint(construid,m_constrs[construid].name,ct,objuids);
        }
    }
}
void georis::Controller::loadFrom(const std::string &fname){
    m_core = Core();

    SVGXReader reader;
    RESCODE res = reader.load(fname.c_str());
    if ( res != RC_OK ){
        MOOLOG << "Controller::loadFrom:  couldn't load file " << fname << std::endl;
        return;
    }


    UID uid = NOUID,uidpar = NOUID;
    ObjectType ot = OT_NONE;
    std::vector<double> params;
    std::string name;
    while ( (res = reader.loadObject(uid,name,ot,params,uidpar) != RC_NO_OBJ )){
        res = m_core.addObject(ot,params,&uid);

        if ( res != RC_OK ){
            MOOLOG << "Controller::loadFrom:  couldn't add object with uid " << uid << std::endl;
            return;
        }
        if ( uidpar != NOUID ){
            // read until parent object is found
            // push child obj UID
            std::vector<UID> chuids;
            chuids.push_back(uid);

            UID par2load = uidpar;
            std::string namepar(name);
            do {
                if ( (res = reader.loadObject(uid,name,ot,params,uidpar)) != RC_OK ){
                    // ACHTUNG !! Not finished reading of parent and siblings
                    MOOLOG << "Controller::loadFrom:  not finished reading of parent " << par2load << " and sibling objs from file" << std::endl;
                    return;
                }
                if ( uidpar == par2load ){
                    res = m_core.addObject(ot,params,&uid);
                    if ( res != RC_OK ){
                        MOOLOG << "Controller::loadFrom:  couldn't add object with uid " << uid << std::endl;
                        return;
                    }
                    EInfo info = {MODE_NORMAL,name};
                    m_objs[uid] = info;
                    // push child objs
                    chuids.push_back(uid);
                }
                else if ( uid == par2load ){
                    // add parent connected to children
                    res = m_core.addObject(ot,params,&uid, &chuids);
                    if ( res != RC_OK ){
                        MOOLOG << "Controller::loadFrom:  couldn't add object with uid " << uid << std::endl;
                        return;
                    }
                    EInfo info = {MODE_NORMAL,namepar};
                    m_objs[uid] = info;
                    break;
                }
                else{
                    // ACHTUNG !!! Found independent object while reading composite object
                    MOOLOG << "Controller::loadFrom:  Found independent object " << uid << " while reading composite object" << par2load << "from file" << std::endl;
                    return;
                }
            }
            while ( true );
        }
        else {
            EInfo info = {MODE_NORMAL,name};
            m_objs[uid] = info;
        }
    }

    std::vector<UID> contrainedObjUIDs;
    ConstraintType ct ;
    double param = 0;
    while ( (res = reader.loadConstraint(uid,name,ct,contrainedObjUIDs,param) != RC_NO_OBJ )){
        res = m_core.addConstraint(ct,contrainedObjUIDs,param,&uid);
        if ( res != RC_OK ){
            MOOLOG << "Controller::loadFrom: Can't add constraint " << uid << std::endl;
            return;
        }
        EInfo info = {MODE_NORMAL,name};
        m_constrs[uid] = info;
    }
    updateView();
}

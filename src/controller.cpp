#include <set>
#include <stack>
#include <iostream>
#include <fstream>
#include <limits>
#include <algorithm>
#include <cassert>

#include "controller.h"
#include "iosvgx.h"
#include "iordb.h"

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

    m_uidActiveSketch = 0;
    m_bIsModified = true;
    m_lastObjNums.clear();
    m_lastConstrNums.clear();
    m_objs.clear();
    m_constrs.clear();
    m_memHighlights[0] = NOUID;

    m_ui->reset();
}

void georis::Controller::setUI(IVisualizer *vis) {
    if ( vis != nullptr ) m_ui = vis;
    showSelectionInfo();
}
void georis::Controller::updateView() {
    //MOOLOG << "Controller::updateView: " << m_objs.size() << " objects to update" << std::endl;
    for (auto &it: m_objs){
        ObjectType objtype;
        std::vector<double> parame;
        size_t freedeg = 0;
        RESCODE res = m_core.queryObjInfo( it.first,objtype,parame,freedeg);
        if ( res != RC_OK ){
            MOOLOG << "Controller::updateView no such obj with uid " << it.first << std::endl;
            return;
        }
        if ( freedeg == 0 ) it.second.status |= MODE_FIXED;
        m_ui->drawObject(objtype,parame,it.second.status);
    }
    for ( auto &c: m_constrs ){
        if ( c.second.type != DCT_NONE ){
            m_ui->displayConstraint(c.second.type,c.second.value,c.second.param(),c.second.status);
        }
    }

    m_ui->reset();
}
void georis::Controller::addObject(georis::ObjectType type, const std::vector<double> &parame,const std::string &name){
    MOOLOG << "Controller::addObject " << type;
    if ( !parame.empty() ) {
        MOOLOG << " with parameters";
        for (size_t k = 0; k< parame.size(); ++k)
            MOOLOG << ' ' << parame[k];
        MOOLOG << std::endl;
    }
    UID uid = NOUID;
    RESCODE res = m_core.addObject(type,parame,&uid);
    if ( res != RC_OK ){
        MOOLOG << "Controller::addObject unsuccesful" << std::endl;
        return;
    }
    MOOLOG << "Controller::addObject added parent with UID " << uid << std::endl;

    const size_t bufsize = 64;
    static char buf[bufsize];

    if (name.empty()) { // Create object's name
        m_lastObjNums[type]++;
        switch(type) {
        case OT_POINT:
            snprintf(buf,bufsize,"Точка%d",m_lastObjNums[type]);
            break;
        case OT_LINE:
            snprintf(buf,bufsize,"Прямая%d",m_lastObjNums[type]);
            break;
        case OT_RAY:
            snprintf(buf,bufsize,"Луч%d",m_lastObjNums[type]);
            break;
        case OT_SEGMENT:
            snprintf(buf,bufsize,"Отрезок%d",m_lastObjNums[type]);
            break;
        case OT_CIRCLE:
            snprintf(buf,bufsize,"Окружность%d",m_lastObjNums[type]);
            break;
        case OT_ARC:
            snprintf(buf,bufsize,"Дуга%d",m_lastObjNums[type]);
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
        MOOLOG << "Controller::addObject can't get children of new object" << std::endl;
        return ;
    }

    for (auto chuid: chids){
        ObjectType ot;
        res = m_core.getObjType(chuid,ot);

        assert( res == RC_OK );

        if ( ot == OT_POINT ){
            m_lastObjNums[OT_POINT]++;
            snprintf(buf,bufsize,"Точка%d",m_lastObjNums[OT_POINT]);
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
        case OT_LINE:
        case OT_RAY:
        case OT_SEGMENT:{
            std::vector<UID> cobjs;
            cobjs.push_back(chids[0]);
            cobjs.push_back(m_memHighlights[0]);
            addConstraint(CT_COINCIDENT,cobjs);
            break;
        }
        case OT_CIRCLE:{
            std::vector<UID> cobjs;
            cobjs.push_back(chids[0]);
            cobjs.push_back(m_memHighlights[0]);
            addConstraint(CT_COINCIDENT,cobjs);
            break;
        }
        case OT_ARC:{
            std::vector<UID> cobjs;
            cobjs.push_back(chids[0]);
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
        case OT_LINE:
        case OT_RAY:
        case OT_SEGMENT:
        case OT_CIRCLE:
        case OT_ARC:{
            std::vector<UID> cobjs;
            cobjs.push_back(chids[1]);
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

RESCODE georis::Controller::addConstraint(ConstraintType type, const std::vector<UID> &objects, double parame, const std::string &name,UID uid){
    for (auto uo: objects ){
        std::vector<double> par;
        m_core.getObjParam(uo,par);
        MOOLOG << "Controller::addConstraint: before id = " << uo << " - ";
        for (auto p: par)
            MOOLOG << p << ", ";
        MOOLOG << std::endl;
    }
    RESCODE res = m_core.addConstraint(type,objects,parame,&uid);
    if ( res != RC_OK ){
        MOOLOG << "Controller::addConstraint " << constrName(type) << " unsuccesful" << std::endl;
        return res;
    }
    MOOLOG << "Controller::addConstraint added " << constrName(type) << " with UID " << uid << std::endl;

    for (auto uo: objects ){
        std::vector<double> par;
        m_core.getObjParam(uo,par);
        MOOLOG << "Controller::addConstraint: after id = " << uo << " - ";
        for (auto p: par)
            MOOLOG << p << ", ";
        MOOLOG << std::endl;
    }



    ECInfo & cinf = m_constrs[uid];
    cinf.type = DCT_NONE;

    m_lastConstrNums[type]++;

    if (name.empty()) { // Create object's name
        const size_t bufsize = 64;
        char buf[bufsize];
        switch(type) {
        case CT_FIX:
            snprintf(buf,bufsize,"Зафиксировано%d",m_lastConstrNums[type]);
            break;
        case CT_EQUAL:
            snprintf(buf,bufsize,"Равно%d",m_lastConstrNums[type]);
            break;
        case CT_VERTICAL:
            snprintf(buf,bufsize,"Вертикально%d",m_lastConstrNums[type]);
            break;
        case CT_HORIZONTAL:
            snprintf(buf,bufsize,"Горизонтально%d",m_lastConstrNums[type]);
            break;
        case CT_DISTANCE:
            snprintf(buf,bufsize,"Расстояние%d",m_lastConstrNums[type]);
            break;
        case CT_ANGLE:
            snprintf(buf,bufsize,"Угол%d",m_lastConstrNums[type]);
            cinf.type = DCT_ANGLEDIM;
            break;
        case CT_PARALLEL:
            snprintf(buf,bufsize,"Параллельно%d",m_lastConstrNums[type]);
            break;
        case CT_ORTHO:
            snprintf(buf,bufsize,"Перпендикулярно%d",m_lastConstrNums[type]);
            break;
        case CT_TANGENT:
            snprintf(buf,bufsize,"Касательно%d",m_lastConstrNums[type]);
            break;
        case CT_COINCIDENT:
            snprintf(buf,bufsize,"Принадлежит%d",m_lastConstrNums[type]);
            break;
        case CT_MIDPOINT:
            snprintf(buf,bufsize,"Середина%d",m_lastConstrNums[type]);
            break;
        case CT_COLLINEAR:
            snprintf(buf,bufsize,"Колинеарно%d",m_lastConstrNums[type]);
            break;
        case CT_DIMENSION:{
            snprintf(buf,bufsize,"Размер%d",m_lastConstrNums[type]);
            break;
        }
        case CT_SYMMETRIC:
            snprintf(buf,bufsize,"Симметрично%d",m_lastConstrNums[type]);
            break;
        case CT_CONCENTRIC:
            snprintf(buf,bufsize,"Концентрично%d",m_lastConstrNums[type]);
            break;
        case CT_ARCBASIC:
            snprintf(buf,bufsize,"ПравильнаяДуга%d",m_lastConstrNums[type]);
        }

        cinf.name = buf;
    }
    else
        cinf.name = name;

    // Create displayed constraint if needed
    cinf.status = 0;
    switch(type) {
    case CT_DISTANCE:{
        cinf.type = DCT_LINDIM;
        cinf.value = parame;
        cinf.status |= MODE_DIMLINE;

        if ( objects.size() == 2 ){
            ObjectType ot1;
            RESCODE res = m_core.getObjType(objects.front(),ot1);
            if ( res != RC_OK ) return res;
            ObjectType ot2;
            res = m_core.getObjType(objects.back(),ot2);
            if ( res != RC_OK ) return res;
            if ( OT_POINT == ot1 && ot1 == ot2){
                cinf.type = DCT_LINDIM;

                linDim* ld = new linDim;
                ld->idbeg = objects.front();
                ld->idend = objects.back();
                ld->dimdx = 0.5;
                ld->dimdy = 0.1;
                ld->core = &m_core;
                cinf.sd.reset(ld);
            }
        }
        break;
    }
    case CT_ANGLE:{
        cinf.type = DCT_ANGLEDIM;
        cinf.value = parame;
        cinf.status |= MODE_DIMLINE;

        std::vector<UID> objchi1;
        RESCODE res = m_core.getObjChilds(objects[0],objchi1);
        if ( res != RC_OK ) return res;
        std::vector<UID> objchi2;
        res = m_core.getObjChilds(objects[1],objchi2);
        if ( res != RC_OK ) return res;

        angDim* andi = new angDim;
        andi->idbeg1 = objchi1[0];
        andi->idend1 = objchi1[1];
        andi->idbeg2 = objchi2[0];
        andi->idend2 = objchi2[1];
        andi->dimr = 1.0;
        andi->dimdir = 0;
        andi->core = &m_core;
        cinf.sd.reset(andi);

        break;
    }
    case CT_DIMENSION:{
        cinf.value = parame;
        cinf.status |= MODE_DIMLINE;
        if ( objects.size() == 1 ){
            ObjectType ot;
            RESCODE res = m_core.getObjType(objects.front(),ot);
            if ( res != RC_OK ) return res;
            switch (ot){
            case OT_SEGMENT:{
                cinf.type = DCT_LINDIM;
                std::vector<UID> objchi;
                res = m_core.getObjChilds(objects.front(),objchi);
                if ( res != RC_OK ) return res;

                linDim* ld = new linDim;
                ld->idbeg = objchi.front();
                ld->idend = objchi.back();
                ld->dimdx = 0.5;
                ld->dimdy = 0.1;
                ld->core = &m_core;
                cinf.sd.reset(ld);

                break;
            }
            case OT_CIRCLE:{
                cinf.type = DCT_CIRCDIM;
                std::vector<UID> objchi;
                res = m_core.getObjChilds(objects.front(),objchi);
                if ( res != RC_OK ) return res;
                std::vector<double> para;
                res = m_core.getObjParam(objects.front(),para);
                if ( res != RC_OK ) return res;

                circDim* cdi = new circDim;
                cdi->idcenter = objchi.front();
                cdi->dimr = para.back();
                cdi->dimdir = 45;
                cdi->offset = 0.7;
                cdi->core = &m_core;
                cinf.sd.reset(cdi);

                break;
            }
            }
        }
        else if ( objects.size() == 2 ){
            ObjectType ot1;
            RESCODE res = m_core.getObjType(objects.front(),ot1);
            if ( res != RC_OK ) return res;
            ObjectType ot2;
            res = m_core.getObjType(objects.back(),ot2);
            if ( res != RC_OK ) return res;
            if ( OT_POINT == ot1 && ot1 == ot2){
                cinf.type = DCT_LINDIM;

                linDim* ld = new linDim;
                ld->idbeg = objects.front();
                ld->idend = objects.back();
                ld->dimdx = 0.5;
                ld->dimdy = 0.1;
                ld->core = &m_core;
                cinf.sd.reset(ld);
            }
        }
        break;
    }
    }
    updateView();
    return RC_OK;
}

void georis::Controller::resetSelection() {
//    MOOLOG << "Controller::resetSelection" << std::endl;
    for (auto && it : m_objs ){
        if ( it.second.status & MODE_SELECTED ){
            it.second.status &= ~MODE_SELECTED;
        }
        if ( it.second.status & MODE_CONSTRAINED )
            it.second.status &= ~MODE_CONSTRAINED;
    }
    m_selectedObjs.clear();
    showSelectionInfo();
}
size_t georis::Controller::selectByPoint(double x,double y,double precision) {
    std::vector<UID> nearest;
    std::vector<double> dists;
    m_core.findObjInCirc(x,y,precision,nearest,&dists);

    if ( nearest.empty() ) {
        resetSelection();        
        return 0;
    }


    std::map<ObjectType,std::map<double,UID> >	 sorter;
    for ( size_t k = 0 ; k < nearest.size();++k ){

        ObjectType ot;
        std::vector<double> parame;
        size_t freedeg = 0;
        m_core.queryObjInfo(nearest[k],ot, parame,freedeg);
        sorter[ot][dists[k]] = nearest[k];

    }

    if ( sorter.find(OT_POINT) != sorter.end() ){
        if ( m_objs[ (*sorter[OT_POINT].begin()).second ].status & MODE_SELECTED ){ // Exclude selected
            for ( size_t k = 0;k < m_selectedObjs.size(); ++k ){
                if ( m_selectedObjs[k] == (*sorter[OT_POINT].begin()).second ){
                    m_selectedObjs.erase(m_selectedObjs.begin() + k);
                    break;
                }
            }
        }
        else{
            m_selectedObjs.push_back((*sorter[OT_POINT].begin()).second);
        }
        m_objs[ (*sorter[OT_POINT].begin()).second ].status ^= MODE_SELECTED;
        showSelectionInfo();
        return 1;
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
    if ( par != NOUID  &&  (m_objs[par].status & MODE_SELECTED) && (m_objs[sel].status & MODE_SELECTED) ) return 0;

    if ( m_objs[sel].status & MODE_SELECTED ){ // Exclude from selected
        for ( size_t k = 0;k < m_selectedObjs.size(); ++k ){
            if ( m_selectedObjs[k] == sel ){
                m_selectedObjs.erase(m_selectedObjs.begin() + k);
                break;
            }
        }
    }
    else {
        m_selectedObjs.push_back(sel);
    }
    m_objs[sel].status ^= MODE_SELECTED;

    unsigned status = m_objs[sel].status;

    std::vector<UID> subs;
    m_core.getObjChilds(sel,subs);
    if ( !subs.empty() )
        for ( size_t s = 0 ; s < subs.size();++s )
            m_objs[subs[s]].status ^= (m_objs[subs[s]].status ^ status) & MODE_SELECTED;

    showSelectionInfo();

    return 1;
}

void georis::Controller::selectByRect(double x1,double y1,double x2,double y2) {
    double xmin = std::min(x1,x2);
    double xmax = std::max(x1,x2);
    double ymin = std::min(y1,y2);
    double ymax = std::max(y1,y2);

    std::vector<UID> nearest;
    m_core.findObjInRect(xmin,ymin,xmax,ymax,nearest);

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
    for (auto &&con: m_constrs )
        if (con.second.status &  MODE_HIGHLIGHTED )
           con.second.status &= ~MODE_HIGHLIGHTED;
}
double georis::Controller::findNearest(double x,double y,std::vector<UID> &objs)const{
    double minDist = std::numeric_limits<double>::infinity();
    for (auto &con: m_constrs){
        if ( nullptr == con.second.sd ) continue;
        double dist = con.second.sd->dist2point(x,y);
        if ( dist > minDist ) continue;
        if ( dist < minDist ) {
            objs.clear();
            minDist = dist;
        }
        objs.push_back(con.first);
    }
    return minDist;
}

void georis::Controller::highlightObj(double x,double y,double precision) {
    resetHighlight();
    std::vector<UID> nearestObjs;
    double minObjDist = m_core.findNearest(x,y,nearestObjs);

    std::vector<UID> nearestConstrs;
    double minConstrDist = findNearest(x,y,nearestConstrs);


    if ( minObjDist <= minConstrDist ){
        if ( minObjDist > precision ) return;

        std::multimap<ObjectType,UID> grouping;

        for ( size_t k = 0 ; k < nearestObjs.size();++k ){
            ObjectType ot;
            m_core.getObjType(nearestObjs[k],ot);
            grouping.insert(std::make_pair(ot,nearestObjs[k]));
        }
        // Highlignt points if any
        auto pointrange = grouping.equal_range(OT_POINT);
        if ( pointrange.first != grouping.end() && pointrange.first->first == OT_POINT ){
            for (auto i = pointrange.first; i != pointrange.second; ++i)
                m_objs[ i->second ].status |= MODE_HIGHLIGHTED;
            return;
        }
        // Highlight first obj in grouping
        m_objs[grouping.begin()->second].status |= MODE_HIGHLIGHTED;
    }
    else{
        if ( minConstrDist > precision ) return;

        // Highlight first constr
        m_constrs[nearestConstrs.front()].status |= MODE_HIGHLIGHTED;
    }

}
void georis::Controller::constrainSelected(ConstraintType type,double parame) {
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
    if ( addConstraint(type,filtered,parame) == RC_OK )
        resetSelection();
}
void georis::Controller::deleteSelected() {
   for (size_t k = 0; k < m_selectedObjs.size(); ++k){
        UID uidpar;
        m_core.getObjParent(m_selectedObjs[k],uidpar);
        if ( uidpar != NOUID ) continue;
        std::vector<UID> subs;
        m_core.getObjChilds(m_selectedObjs[k],subs);
        // Prepare to update m_constrs
        std::vector<UID> construids;
        m_core.getObjConstraints(m_selectedObjs[k],construids);

        m_core.removeObject(m_selectedObjs[k]);
        for ( size_t s = 0 ; s < subs.size();++s )
            m_objs.erase(subs[s]);
        m_objs.erase(m_selectedObjs[k]);

        for (auto cuid: construids){
            auto cit = m_constrs.find(cuid);
            m_constrs.erase(cit);
        }

    }
    m_selectedObjs.clear();

    showSelectionInfo();
}
void georis::Controller::findObj(unsigned stateMask,std::vector<UID> &res){
    res.clear();
    for (auto &it : m_objs )
        if ( it.second.status & stateMask )
            res.push_back(it.first);
}

void georis::Controller::moveSelected(double dx,double dy) {
    if ( !m_selectedObjs.empty() ){
        if ( m_selectedObjs.size() == 1 ){
            ObjectType ot = OT_NONE;
            std::vector<double> parame;
            size_t fd = 0;
            m_core.queryObjInfo(m_selectedObjs.front(),ot,parame,fd );
            if ( ot == OT_CIRCLE ){
                // Project displacement onto readius vector to determine radius change
                double rvx = m_xSel - parame[0];
                double rvy = m_ySel - parame[1];
                double dr = (dx*rvx + dy*rvy)/parame[2];
                parame[2] += dr;
                m_core.setObjParam(m_selectedObjs.front() ,parame );
            }

        }
        else
            m_core.moveObjects(m_selectedObjs,dx,dy);
    }
}
void georis::Controller::processDrag(double x, double y) {

    if ( !m_selectedObjs.empty() ){
        if ( m_selectedObjs.size() == 1 ){
            ObjectType ot = OT_NONE;
            std::vector<double> parame;
            size_t fd = 0;
            m_core.queryObjInfo(m_selectedObjs.front(),ot,parame,fd );
            if ( ot == OT_CIRCLE ){
                double rvx = x - parame[0];
                double rvy = y - parame[1];
                parame[2] = sqrt(rvx*rvx + rvy*rvy);
                m_core.setObjParam(m_selectedObjs.front() ,parame );
                return;
            }
            m_core.moveObjects(m_selectedObjs,x - m_xPrev,y - m_yPrev);
        }
        else
            m_core.moveObjects(m_selectedObjs,x - m_xPrev,y - m_yPrev);
    }
    m_xPrev = x; m_yPrev = y;
}

void georis::Controller::toggleAuxSelected(){
    std::vector<UID> selected;
    findObj(MODE_SELECTED,selected);
    for (auto objid: selected)
        m_objs[objid].status ^= MODE_CONSTRUCTI;
    resetSelection();
}

void georis::Controller::showSelectionInfo() {
    if ( m_ui == nullptr ) return;
    // Collect selected objects info

    std::vector<std::pair<UID,std::string> > objsSel;
    std::vector<ConstraintType>  constrsAvail;
    std::vector<std::pair<UID,std::string>> constrsSel;

    if ( !m_selectedObjs.empty() ){
        std::vector<UID> selected(m_selectedObjs);
        m_core.filterChildObj(selected);
        objsSel.reserve(selected.size());
        std::map<ObjectType,size_t> nSel;
        size_t totalfree = 0;
        for (auto it : selected){
            ObjectType ot;
            std::vector<double> parame;
            size_t fd = 0;
            m_core.queryObjInfo(it,ot,parame,fd);
            totalfree += fd?1:0;
            ++nSel[ot];
            objsSel.push_back( std::make_pair(it,m_objs[it].name) );
        }

        MOOLOG << "Controller::showSelectionInfo: " << m_objs.size() << " total objects, selected "<< selected.size() << std::endl;
        MOOLOG << "  nPoints = "<< nSel[OT_POINT] << std::endl;
        MOOLOG << "  nLines = " << nSel[OT_LINE] << std::endl;
        MOOLOG << "  nRays = " << nSel[OT_RAY] << std::endl;
        MOOLOG << "  nSegs = " << nSel[OT_SEGMENT] << std::endl;
        MOOLOG << "  nCircls = " << nSel[OT_CIRCLE] << std::endl;
        MOOLOG << "  nArcs = " << nSel[OT_ARC] << std::endl;

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



        constrsSel.reserve(commonSelConstrs.size());
        for (auto it : commonSelConstrs )
            constrsSel.push_back(std::make_pair(it,m_constrs[it].name));


        // Show available constraints

        if ( totalfree != 0 ) {
            if ( totalfree == selected.size() ) constrsAvail.push_back(CT_FIX);

            if ( m_selectedObjs.size() > 1 && m_selectedObjs.size() == nSel[OT_POINT] ) // Only points selected
                constrsAvail.push_back(CT_COINCIDENT);

            if ( nSel[OT_POINT] == 0 &&
                 (nSel[OT_LINE] || nSel[OT_RAY] || nSel[OT_SEGMENT]) &&
                 nSel[OT_CIRCLE] == 0 &&
                 nSel[OT_ARC] == 0) {
                constrsAvail.push_back(CT_VERTICAL);
                constrsAvail.push_back(CT_HORIZONTAL);
                if ( nSel[OT_LINE] ==0 && nSel[OT_RAY] ==0 ){
                    if ( nSel[OT_SEGMENT] == 1)
                        constrsAvail.push_back(CT_DIMENSION);
                    else
                        constrsAvail.push_back(CT_EQUAL);
                }
                if ( (nSel[OT_LINE] + nSel[OT_RAY] + nSel[OT_SEGMENT]) > 1 ){
                    constrsAvail.push_back(CT_PARALLEL);
                    constrsAvail.push_back(CT_COLLINEAR);
                }
                if ( (nSel[OT_LINE] + nSel[OT_RAY] + nSel[OT_SEGMENT]) == 2 ){
                    constrsAvail.push_back(CT_ANGLE);
                    constrsAvail.push_back(CT_ORTHO);
                }
            }

            if ( nSel[OT_POINT] == 0 &&
                 nSel[OT_LINE] == 0 &&
                 nSel[OT_RAY] == 0 &&
                 nSel[OT_SEGMENT] == 0 &&
                 (nSel[OT_CIRCLE] || nSel[OT_ARC])) { // Only circles selected
                if ( (nSel[OT_CIRCLE] ==1 && nSel[OT_ARC] == 0) || (nSel[OT_CIRCLE]==0 && nSel[OT_ARC] == 1) )
                    constrsAvail.push_back(CT_DIMENSION);
                else{
                    constrsAvail.push_back(CT_EQUAL);
                    constrsAvail.push_back(CT_CONCENTRIC);
                    constrsAvail.push_back(CT_TANGENT);
                }
            }
            if ( nSel[OT_POINT] == 1 ){
                if ( (nSel[OT_LINE] + nSel[OT_RAY] + nSel[OT_SEGMENT] + nSel[OT_CIRCLE] + nSel[OT_ARC]) == 1)
                    constrsAvail.push_back(CT_COINCIDENT);
                if ( nSel[OT_SEGMENT] == 1 && (nSel[OT_LINE] + nSel[OT_RAY] + nSel[OT_CIRCLE] + nSel[OT_ARC]) == 0 )
                    constrsAvail.push_back(CT_MIDPOINT);
            }
            if ( nSel[OT_POINT] == 2 && (nSel[OT_LINE] + nSel[OT_RAY] + nSel[OT_SEGMENT] + nSel[OT_CIRCLE] + nSel[OT_ARC]) ==  0 )
                constrsAvail.push_back(CT_DIMENSION);
            if ( (nSel[OT_LINE] + nSel[OT_RAY] + nSel[OT_SEGMENT]) == 1 ){
                if ( nSel[OT_POINT] == 2 && (nSel[OT_CIRCLE] + nSel[OT_ARC]) == 0 )
                    constrsAvail.push_back(CT_SYMMETRIC);
                if ( nSel[OT_POINT] == 0 && (nSel[OT_CIRCLE] + nSel[OT_ARC]) == 1 )
                    constrsAvail.push_back(CT_TANGENT);
            }
        }
    }
    m_ui->setSelectionInfo(objsSel,constrsSel,constrsAvail);

}
void georis::Controller::memHighlightsDown(double x,double y){
    m_memHighlights[0] = NOUID;
    m_xSel = x; m_ySel = y;
    m_xPrev = x; m_yPrev = y;
    std::vector<UID> highlighted;
    findObj(MODE_HIGHLIGHTED,highlighted);
    if ( !highlighted.empty() )
        m_memHighlights[0] = highlighted[0];
//    MOOLOG << "Controller::memHighlightsDown " << m_memHighlights[0] << std::endl;
}
void georis::Controller::memHighlightsUp(double x,double y){
    m_memHighlights[1] = NOUID;
    std::vector<UID> highlighted;
    findObj(MODE_HIGHLIGHTED,highlighted);
    if ( !highlighted.empty() )
        m_memHighlights[1] = highlighted[0];
//    MOOLOG << "Controller::memHighlightsUp " << m_memHighlights[1] << std::endl;
}
void georis::Controller::highlightConstrainedBy(UID constrid,bool highlight){
    // find objects
    ConstraintType ctype;
    std::vector<UID> constrobjs;
    if ( m_core.queryConstrInfo(constrid,ctype,constrobjs) == RC_NO_OBJ ) return;
    for (auto objid: constrobjs ){
        if ( highlight )
            m_objs[objid].status |= MODE_CONSTRAINED;
        else
            m_objs[objid].status &= ~MODE_CONSTRAINED;
    }
    updateView();
}
void georis::Controller::saveTo(const std::string &fname){
    std::vector<UID> objuids;
    m_core.enumObjs(objuids);
    m_core.filterChildObj(objuids);

    std::unique_ptr<IFileWriter> writer;
    if ( fname.back() == 'l' ){ // SQL writer
        writer.reset(new RDBWriter);
    }
    else if ( fname.back() == 'x'){
        writer.reset(new SVGXWriter);
    }
    else {
        MOOLOG << "Controller::saveTo: undefined file format" << fname << std::endl;
        return;
    }

    RESCODE res = writer->prepare(fname.c_str());
    if ( res != RC_OK ){
        MOOLOG << "Controller::saveTo: can't prepare file " << fname << std::endl;
        return;
    }

    for (auto uid:objuids){
        std::vector<UID> chuids;
        m_core.getObjChilds(uid,chuids);
        ObjectType ot;
        std::vector<double> parame;
        size_t freedeg = 0;
        for (auto chuid:chuids){
            res = m_core.queryObjInfo(chuid,ot,parame,freedeg);
            if ( res != RC_OK ){
                MOOLOG << "Controller::saveTo: no obj uid " << chuid << std::endl;
                return;
            }
            res = writer->saveObject(chuid,m_objs[chuid].name,ot,parame,m_objs[chuid].status,uid);
            if ( res != RC_OK ){
                MOOLOG << "Controller::saveTo: can't save obj with uid " << chuid << std::endl;
                return;
            }
        }
        res = m_core.queryObjInfo(uid,ot,parame,freedeg);
        if ( res != RC_OK ){
            MOOLOG << "Controller::saveTo: no obj uid " << uid << std::endl;
            return;
        }
        res = writer->saveObject(uid,m_objs[uid].name,ot,parame,m_objs[uid].status);
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
            writer->saveConstraint(construid,m_constrs[construid].name,ct,objuids,&para);
            break;
        default:
            writer->saveConstraint(construid,m_constrs[construid].name,ct,objuids);
        }
    }
}
void georis::Controller::loadFrom(const std::string &fname){    
    m_core = Core();
    clearAll();

    SVGXReader reader;
    RESCODE res = reader.load(fname.c_str());
    if ( res != RC_OK ){
        MOOLOG << "Controller::loadFrom:  couldn't load file " << fname << std::endl;
        return;
    }


    UID uid = NOUID,uidpar = NOUID,maxuid = NOUID;
    ObjectType ot = OT_NONE;
    std::vector<double> params;
    std::string name;
    unsigned attributes;
    while ( reader.loadObject(uid,name,ot,params,attributes,uidpar) != RC_NO_OBJ ){
        if ( maxuid < uid ) maxuid = uid;
        res = m_core.addObject(ot,params,&uid);

        if ( res != RC_OK ){
            MOOLOG << "Controller::loadFrom:  couldn't add object with uid " << uid << std::endl;
            return;
        }

        EInfo info = {MODE_NORMAL,name};
        m_objs[uid] = info;

        if ( uidpar != NOUID ){
            // read until parent object is found
            // push child obj UID
            std::vector<UID> chuids;
            chuids.push_back(uid);

            UID par2load = uidpar;
            do {
                unsigned attributes;
                if ( (res = reader.loadObject(uid,name,ot,params,attributes,uidpar)) != RC_OK ){
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
                    EInfo info = {MODE_NORMAL,name};
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
    }

    std::vector<UID> constrainedObjUIDs;
    ConstraintType ct ;
    double parame = 0;
    while ( (res = reader.loadConstraint(uid,name,ct,constrainedObjUIDs,parame) != RC_NO_OBJ )){

        res = addConstraint(ct,constrainedObjUIDs,parame,name,uid);
        if ( res != RC_OK ){
            MOOLOG << "Controller::loadFrom: Can't add constraint " << uid << std::endl;
            return;
        }
    }
    MOOLOG << "Controller::loadFrom: latest UID " << uid << std::endl;

    UIDGen::instance()->init(reader.getLastUID());

    updateView();
}
const char* georis::Controller::constrName(georis::ConstraintType ct){
    static const char* names[] = {
        "CT_FIX",
        "CT_EQUAL",
        "CT_VERTICAL",
        "CT_HORIZONTAL",
        "CT_DISTANCE",
        "CT_ANGLE",
        "CT_PARALLEL",
        "CT_ORTHO",
        "CT_TANGENT",
        "CT_COINCIDENT",
        "CT_MIDPOINT",
        "CT_COLLINEAR",
        "CT_DIMENSION",
        "CT_SYMMETRIC",
        "CT_CONCENTRIC",
        "CT_ARCBASIC"

    };
    return names[static_cast<size_t>(ct)];
}

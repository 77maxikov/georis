#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "IVisualizer.h"
#include <vector>
#include <iostream>

#include "core.h"
#include "dimline.h"

namespace georis{
class Controller {
public:
    Controller();
    ~Controller();
    void setUI(IVisualizer *);
    void updateView();

    void addObject(ObjectType type, const std::vector<double> &parame,const std::string &name = std::string());

    size_t selectByPoint(double x,double y,double precision = 0.01);
    void selectByRect(double x1,double y1,double x2,double y2);
    void resetSelection();

    void deleteSelected();
//    void moveSelected(double dx,double dy);

    void constrainSelected(ConstraintType type,double parame = 0);
    void toggleAuxSelected();

    void highlightObj(double x,double y,double precision = 0.01);

    void memHighlightsDown(double x,double y);
    void memHighlightsUp(double x,double y);
    void processDrag(double x,double y);
    void resetHighlight();

    void highlightConstrainedBy(UID,bool);

    void createNew();
    void saveTo(const std::string &fname);
    void loadFrom(const std::string &fname);

    static const char* constrName(ConstraintType ct);
protected:
private:
    IVisualizer *m_ui;
    Core m_core;

    UID m_uidActiveSketch;

    bool m_bIsModified;

    double m_xSel, m_ySel;
    double m_xPrev, m_yPrev;
    std::vector<UID> m_selectedObjs;

    struct EInfo{
        unsigned status;
        std::string name;
    };

    struct ECInfo:public EInfo{
        ECInfo():type(DCT_NONE),value(0.0),sd(nullptr){}
        DCType type;
        double value;
        std::unique_ptr<sketchDim> sd;

        std::vector<double> param()const{
            return (*sd).param();
        };

    };

    std::map<ObjectType,int> m_lastObjNums;

    std::map<ConstraintType,int> m_lastConstrNums;

    std::map<UID,EInfo> m_objs;
    std::map<UID,ECInfo> m_constrs;

    UID m_memHighlights[2];

    void findObj(unsigned stateMask,std::vector<UID> &res);
    void showSelectionInfo();
    double findObjInCirc(double x,double y,double radius,std::vector<UID> &objs,std::vector<double> *pdists = nullptr)const;

    double findNearest(double x,double y,std::vector<UID> &objs)const;

    RESCODE addConstraint(ConstraintType type, const std::vector<UID> &objects,double parame = 0,const std::string &name = std::string(),UID id = NOUID);
    RESCODE addSimpleObj(ObjectType type, const std::vector<double> &parame,const std::string &name,std::vector<UID>* newids = nullptr);

    void clearAll(){
        m_memHighlights[0] = NOUID;
        m_memHighlights[1] = NOUID;
        m_lastObjNums.clear();
        m_lastConstrNums.clear();
        m_objs.clear();
        m_constrs.clear();
    }

    static const double _sel_precision;
    static bool isSimpleObj(ObjectType typ){
        switch (typ){
        case OT_ARC:
        case OT_CIRCLE:
        case OT_LINE:
        case OT_POINT:
        case OT_RAY:
        case OT_SEGMENT:
        case OT_SPLINE:
            return true;
        }
        return false;
    }
};
}

#endif // CONTROLLER_H

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "IVisualizer.h"
#include <vector>
#include <string>
#include <stack>

#include "core.h"

namespace georis{
class Controller {
public:
    Controller();
    ~Controller();
    void setUI(IVisualizer *);
    void updateView();

    void addObject(ObjectType type, const std::vector<double> &param,const std::string &name = std::string());

    size_t selectByPoint(double x,double y,double precision = 0.01);
    void selectByRect(double x1,double y1,double x2,double y2);

    void deleteSelected();
    void moveSelected(double dx,double dy);
    void constrainSelected(ConstraintType type,double param = 0);

    void highlightObj(double x,double y,double precision = 0.01);
    void resetSelection();
    void memHighlightsDown();
    void memHighlightsUp();
    void resetHighlight();

    void createNew();
    void saveTo(const std::string &fname);
    void loadFrom(const std::string &fname);

protected:
private:
    IVisualizer *m_ui;
    Core m_core;

    UID m_uidActiveSketch;

    bool m_bIsModified;

    struct EInfo{
        unsigned status;
        std::string name;
    };

    std::map<ObjectType,int> m_lastObjNums;

    std::map<ConstraintType,int> m_lastConstrNums;

    std::map<UID,EInfo> m_objs;
    std::map<UID,EInfo> m_constrs;

    UID m_memHighlights[2];

    void findObj(unsigned stateMask,std::vector<UID> &res);

    void showSelectionInfo();

    void addConstraint(ConstraintType type, const std::vector<UID> &objects,double param = 0,const std::string &name = std::string());

    void clearAll(){
        m_memHighlights[0] = NOUID;
        m_memHighlights[1] = NOUID;
        m_lastObjNums.clear();
        m_lastConstrNums.clear();
        m_objs.clear();
        m_constrs.clear();
    }

    static const double _sel_precision;
};
}

#endif // CONTROLLER_H

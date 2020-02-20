#ifndef _GEOSCONTROLLER_H
#define _GEOSCONTROLLER_H
#include "IVisualizer.h"
#include <vector>
#include <string>
#include <stack>

#include "core.h"

class GeosController {
public:
    GeosController();
    ~GeosController();
    void setUI(IVisualizer *);
    void updateView();

    void addObject(georis::ObjectType type, const std::vector<double> &param,const std::string &name = std::string());

    void selectByPoint(double x,double y,double precision = 0.01);
    void selectByRect(double x1,double y1,double x2,double y2);

    void deleteSelected();
    void moveSelected(double dx,double dy);
    void constrainSelected(georis::ConstraintType type,double param = 0);

    void highlightObj(double x,double y,double precision = 0.01);
    void memHighlightsDown();
    void memHighlightsUp();

	void saveTo(const std::string &fname);
	void loadFrom(const std::string &fname);

protected:
private:
    IVisualizer *_ui;
        georis::Core _core;
	struct EInfo{
		unsigned status;
		std::string name;
	};
        std::map<georis::ObjectType,int> _lastObjNums;

	std::map<UID,EInfo> _objs;
	std::map<UID,EInfo> _constrs;

	UID _memHighlights[2];

	void findObj(unsigned stateMask,std::vector<UID> &res);
    void resetSelection();
    void resetHighlight();

    void showSelectionInfo();

    static const double _sel_precision;
};

#endif // _GEOSCONTROLLER_H

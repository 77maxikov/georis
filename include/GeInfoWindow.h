#ifndef GEINFOWINDOW_H
#define GEINFOWINDOW_H
#include "FL/Fl_Window.h"
#include "FL/Fl_Multi_Browser.H"
#include "FL/Fl_Hold_Browser.H"
#include "Fl_Toolbar.h"
#include <map>
#include <vector>
#include <string>
#include "uid.h"
#include "objects.h"


class GeInfoWindow: public Fl_Window
{
public:
   GeInfoWindow(int x,int y,int w,int h,const char *l=0);
   void setController(georis::Controller *controller) {m_controller = controller;}
   void setSelectionInfo(const std::vector<std::pair<UID,std::string> > &objsSel,
                         const std::vector< std::pair<UID,std::string> > &constrsSel,
                         const std::vector<georis::ConstraintType>& constrsAvail);

   static void cbConstrVertical(Fl_Widget*w, void*d);
   static void cbConstrHorizontal(Fl_Widget*w, void*d);
   static void cbConstrParallel(Fl_Widget*w, void*d);
   static void cbConstrOrtho(Fl_Widget*w, void*d);
   static void cbConstrCoincident(Fl_Widget*w, void*d);
   static void cbConstrCollinear(Fl_Widget*w, void*d);
   static void cbConstrTangent(Fl_Widget*w, void*d);
   static void cbConstrEqual(Fl_Widget*w, void*d);
   static void cbConstrFix(Fl_Widget*w, void*d);
   static void cbConstrDim(Fl_Widget*w, void*d);
   static void cbConstrAngle(Fl_Widget*w, void*d);
   static void cbConstrSymmetric(Fl_Widget*w, void*d);
   static void cbConstrMidpoint(Fl_Widget*w, void*d);
   static void cbConstrConcentric(Fl_Widget*w, void*d);

   static void cbToggleAux(Fl_Widget*w, void*d);

   static void cbSelectConstr(Fl_Widget*w, void*d);
   static void cbDeleteConstr(Fl_Widget*w, void*d);


private:
   georis::Controller *m_controller;

   Fl_Multi_Browser *m_pBrowserSelected;
   Fl_Hold_Browser *m_pBrowserComCon;

   Fl_Toolbar *m_pActionToolbar;
   Fl_Button *m_pToggleAuxButton;
   std::map<georis::ConstraintType,Fl_Image_Button*> m_constrbuttons;

   std::vector<UID> m_currentConstraints;
};

#endif // GEINFOWINDOW_H

#ifndef GEINFOWINDOW_H
#define GEINFOWINDOW_H
#include "FL/Fl_Window.h"
#include "FL/Fl_Multi_Browser.H"
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
   void setController(georis::Controller *controller) {_controller = controller;}
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

private:
   georis::Controller *_controller;

   Fl_Multi_Browser *m_pBrowserSelected;
   Fl_Multi_Browser *m_pBrowserComCon;

   Fl_Toolbar *m_pActionToolbar;

   std::map<georis::ConstraintType,Fl_Image_Button*> _constrbuttons;
};

#endif // GEINFOWINDOW_H

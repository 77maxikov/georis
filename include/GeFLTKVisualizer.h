#ifndef _GEFLTKVISUALIZER_H
#define _GEFLTKVISUALIZER_H

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include "Fl_Toolbar.h"

#include "IVisualizer.h"
//#include "uid.h"
#include "GeGlWindow.h"
#include "GeInfoWindow.h"
#include <map>
using std::map;

class GeFLTKVisualizer:public Fl_Double_Window,public IVisualizer {
public:
   GeFLTKVisualizer(int W = 1200, int H = 800, const char*L = "Geos");
   ~GeFLTKVisualizer();

   void resize(int x, int y, int w, int h);

   virtual void reset(){
      _glWindow->redraw();
   }
   virtual void setController(georis::Controller *ctrl);
   virtual void drawPoint(const double *x, const double *y,unsigned status = MODE_NORMAL);
   virtual void drawLine(const double *x1, const double *y1, const double *x2, const double *y2, unsigned status = MODE_NORMAL );
   virtual void drawCircle(const double *x, const double *y, const double *r,unsigned status = MODE_NORMAL );
   virtual void setSelectedObjs(const std::map<UID,std::string> &);
   virtual void setSelectedConstraints(const std::map<UID,std::string> &);
   virtual void setAvailConstraints(const std::vector<georis::ConstraintType>&);

   static void cbFileNew(Fl_Widget*, void*);
   static void cbFileOpen(Fl_Widget*, void*);
   static void cbFileSave(Fl_Widget*, void*);
   static void cbUndo(Fl_Widget*, void*);
   static void cbRedo(Fl_Widget*, void*);

   static void cbDrawPoint(Fl_Widget*, void*);
   static void cbDrawLine(Fl_Widget*, void*);
   static void cbDrawCircle(Fl_Widget*, void*);
   static void cbDrawArc(Fl_Widget*, void*);
   static void cbDrawRect(Fl_Widget*, void*);

private:
   georis::Controller *_controller;
   GeGlWindow *_glWindow;
   Fl_Toolbar *_toolbar;
   GeInfoWindow *_infowin;


   int handle(int event);
   int processKeyboard(int key);
   void setInputMode(InputMode mode);

   map<InputMode,Fl_ImageCheckButton*> _modebuttons;
};

#endif // _GEFLTKVISUALIZER_H

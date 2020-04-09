#ifndef _GEFLTKVISUALIZER_H
#define _GEFLTKVISUALIZER_H

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include "Fl_Toolbar.h"
#include "FL/Fl_Box.h"

#include "IVisualizer.h"
//#include "uid.h"
#include "GeGlWindow.h"
#include "GeInfoWindow.h"
#include <map>
using std::map;

namespace georis{
class GeFLTKVisualizer:public Fl_Double_Window,public IVisualizer {
public:
   GeFLTKVisualizer(int W = 1200, int H = 800, const char*L = "Geos");
   ~GeFLTKVisualizer();

   void resize(int x, int y, int w, int h);

   virtual void reset(){
      _glWindow->redraw();
   }
   void setStatusMessage(const char *msg);

   virtual void setController(georis::Controller *ctrl);
   virtual void drawObject(ObjectType type, const std::vector<double> &param,unsigned status = MODE_NORMAL);

   virtual void setSelectionInfo(const std::vector<std::pair<UID,std::string> > &objsSel,
                            const std::vector< std::pair<UID,std::string> > &constrsSel,
                            const std::vector<georis::ConstraintType>& constrsAvail);

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
   Fl_Box *_statusbar;


   int handle(int event);
   int processKeyboard(int key);
   void setInputMode(InputMode mode);

   map<InputMode,Fl_ImageCheckButton*> _modebuttons;
};
}
#endif // _GEFLTKVISUALIZER_H

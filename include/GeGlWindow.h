#ifndef _GEGLWINDOW_H
#define _GEGLWINDOW_H
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Box.H>
#include "controller.h"
#include "fontdata.h"
#include "objects.h"

class GeGlWindow:public Fl_Gl_Window{
   public:
      GeGlWindow(int x,int y, int w,int h,const char *name=0);
      void setController(georis::Controller *c){if (c) m_controller = c;};
      //void setStatusBar(Fl_Box *b){m_statusbar = b;};
      void drawPoint(double x, double y,unsigned status );
      void drawSegment(double x1, double y1, double x2, double y2,unsigned status );
      void drawRay(double x1, double y1, double x2, double y2,unsigned status );
      void drawLine(double x1, double y1, double x2, double y2,unsigned status );
      void drawCircle(double x, double y, double r,unsigned status );
      //void drawArc(double cx, double cy, double px,double py, double angle,unsigned status);
      void drawArc(double cx, double cy, double bx,double by, double ex, double ey,unsigned status);
      void drawDimLine(double x1, double y1, double x2, double y2, double x3, double y3,const char *dimstr,unsigned status);
      void drawRadiusDimLine(double cx, double cy, double r, double dir, double offset,const char *dimstr,unsigned status);
      void drawAngleDimLine(double cx, double cy,double bdir, double edir,  double r, double textdir,const char *dimstr,unsigned status);


      void setMode(georis::InputMode mode){m_input_mode = mode; m_state = 0;};
      virtual void resize(int x,int y, int w,int h);

   private:
      //Fl_Box *m_statusbar;

      georis::Controller *m_controller;
      georis::FontData m_fontdata;

      float m_left, m_right, m_top, m_bottom;
      double m_coo[8];// m_coo[0:1] - current position

      const double m_zoomFactor = 0.9;
      double m_currentZoom = 0;
      unsigned m_input_mode;
      unsigned m_state;

      void draw();
      void setColor(unsigned status);
      void setStyle(unsigned status);
      void clearBG();
      void showGrid();
      void showMode();
      void showInfo();
      void setZoom();
      void win2int(int wx,int wy,double &ix,double &iy);
      void processMotion();
      void processMouse(int button, int state, int x, int y);
      void processDrag(int x,int y);
      void processMove(int x,int y);
      void zoomIn(double px,double py);
      void zoomOut(double px,double py);
      void updateCurrentPos(double x, double y);
      georis::AABBr getScreenBox()const;

      void intDrawArc(double cx, double cy, double bx,double by, double ex, double ey);

      int handle(int event);

      double minPixelRes()const{return std::min((m_right - m_left)/w(), (m_top - m_bottom)/h());};
};

#endif // _GEGLWINDOW_H

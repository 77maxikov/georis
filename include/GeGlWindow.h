#ifndef _GEGLWINDOW_H
#define _GEGLWINDOW_H
#include <FL/Fl_Gl_Window.H>
#include "controller.h"

class GeGlWindow:public Fl_Gl_Window{
   public:
      GeGlWindow(int x,int y, int w,int h,const char *name=0);
      ~GeGlWindow();
      void setController(georis::Controller *c){if (c) _controller = c;};
      void drawPoint(double x, double y,unsigned status );
      void drawLine(double x1, double y1, double x2, double y2,unsigned status );
      void drawCircle(double x, double y, double r,unsigned status );
      //void drawArc(double cx, double cy, double px,double py, double angle,unsigned status);
      void drawArc(double cx, double cy, double bx,double by, double ex, double ey,unsigned status);
      void setMode(georis::InputMode mode){_input_mode = mode; _state = 0;};
      virtual void resize(int x,int y, int w,int h);
   private:
      georis::Controller *_controller;
      float _left, _right, _top, _bottom;      
      double _coo[8];// _coo[0:1] - current _coo[]
      double _zoomFactor;
      unsigned _input_mode;
      unsigned _state;

      void draw();
      void setColor(unsigned status);
      void setStyle(unsigned status);
      void clearBG();
      void showGrid();
      void showMode();
      void setZoom();
      void win2int(int wx,int wy,double &ix,double &iy);
      void processMotion();
      void processMouse(int button, int state, int x, int y);
      void processDrag(int x,int y);
      void processMove(int x,int y);
      void zoomIn(double px,double py);
      void zoomOut(double px,double py);


      int handle(int event);

      double minPixelRes()const{return std::min((_right - _left)/w(), (_top - _bottom)/h());};
};

#endif // _GEGLWINDOW_H

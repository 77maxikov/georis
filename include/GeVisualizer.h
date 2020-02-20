#ifndef _GEOVISUALIZER_H
#define _GEOVISUALIZER_H
#include <GL/glut.h>
#include "IVisualizer.h"
#include "controller.h"

class GeVisualizer:public IVisualizer
{
    static GeVisualizer *_instance;

    static void displayFunc(void);
    static void keyboardFunc(GLubyte key,GLint x,GLint y);
    static void specialFunc(GLint key,GLint x,GLint y);
    static void mouseFunc(int button, int state, int x, int y);
    static void motionFunc(int x, int y);

    GeVisualizer();
    unsigned _input_mode;
    unsigned _state;
    bool _selected;
    bool _showGrid;
    UID _lastSelUid;


    GLfloat _left, _right, _top, _bottom;
    GLfloat bgcR, bgcG, bgcB;
    GLfloat objcR, objcG, objcB;
	GLfloat objcRsel, objcGsel, objcBsel;
	GLfloat objcRfix, objcGfix, objcBfix;
	GLfloat objcRfixsel, objcGfixsel, objcBfixsel;


    georis::Controller *_controller;
    double _begx,_begy;
    double _curx,_cury;
    double _zoomFactor;

    void show();
    void processKeyboard(GLubyte key,GLint x,GLint y);
    void processMouse(int button, int state, int x, int y);
    void processMotion(int x,int y);
    void showGrid();
    void clear();
    void setZoom();
    void zoomIn(double px,double py);
    void zoomOut(double px,double py);
    void showMode();
    void win2int(int wx,int wy,double &ix,double &iy);
    void setColor(bool selected,bool fixed);
    void drawAALine( double x1, double y1, double x2, double y2, float w, //width/thickness of the line in pixel
		  float Cr, float Cg, float Cb,
		  float Br, float Bg, float Bb,
		  bool alphablend);
	 double minPixelRes()const{return std::min((_right - _left)/glutGet(GLUT_WINDOW_WIDTH), (_top - _bottom)/glutGet(GLUT_WINDOW_HEIGHT));};

public:
    static GeVisualizer* instance()
    {
        if (_instance == nullptr)
            _instance = new GeVisualizer;
        return _instance;
    }
    void init(int argc, char* argv[]);
    void execute();

    void reset(){_showGrid = true;}
    virtual void setController(georis::Controller *);
    virtual void drawPoint(const double *px,const double *py,unsigned status = MODE_NORMAL);
    virtual void drawLine(const double *px1,const double *py1,const double *px2,const double *py2,unsigned status = MODE_NORMAL);
    virtual void drawCircle(const double *px,const double *py,const double *r,unsigned status = MODE_NORMAL);
    virtual void setAvailConstraints(const std::vector<georis::ConstraintType> &){};
    virtual void setSelectedObjs(const std::map<UID,std::string> &){};
    virtual void setSelectedConstraints(const std::map<UID,std::string> &){};

};

#endif // _GEOVISUALIZER_H

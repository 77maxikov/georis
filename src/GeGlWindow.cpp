#include "GeGlWindow.h"
#include <FL/gl.h>
#include <FL/Fl.H>
#include "mooLog.h"
#include "georis.h"
#include <cmath>
#include <iostream>

GeGlWindow::GeGlWindow(int x,int y, int w,int h,const char *name):Fl_Gl_Window(x,y,w,h,name) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//ctor
    if ( w <= 0 ) throw std::invalid_argument("GeGlWindow::bad width");
    _left = -1; _right = 1; _top = static_cast<float>(h)/w; _bottom = -static_cast<float>(h)/w;
    glViewport(0,0,w,h);
    memset( _coo,0,sizeof(_coo) );
    _zoomFactor = 0.9;
	_input_mode = IM_NONE;
	_state = 0;
}

GeGlWindow::~GeGlWindow() {
    //dtor
}
void GeGlWindow::resize(int x,int y, int ww,int hh){
	float wdelta = (ww*(_right - _left)/w() - (_right-_left))/2;
	float hdelta = (hh*(_top - _bottom)/h() - (_top - _bottom))/2;
    _left -=  wdelta;
    _right += wdelta;
    _top +=  hdelta;
    _bottom -=  hdelta;
	glViewport(0,0,ww,hh);
	Fl_Gl_Window::resize(x,y,ww,hh);
}
int GeGlWindow::handle(int event){
   switch (event){
		case FL_PUSH:
           MOOLOG << "GeGlWindow::handle Mouse Pushed x = " << Fl::event_x() << " y = " << Fl::event_y() << std::endl;
		   processMouse(Fl::event_button(),FL_PUSH,Fl::event_x(),Fl::event_y());
		   return 1;
      case FL_RELEASE:
           MOOLOG << "GeGlWindow::handle Mouse Released x = " << Fl::event_x() << " y = " << Fl::event_y() << std::endl;
		   processMouse(Fl::event_button(),FL_RELEASE,Fl::event_x(),Fl::event_y());
		   break;
		case FL_MOVE:
			processMove(Fl::event_x(),Fl::event_y());
			break;
		case FL_DRAG:
			processDrag(Fl::event_x(),Fl::event_y());
			break;
		case FL_MOUSEWHEEL:
			processMouse(Fl::event_button(),FL_MOUSEWHEEL,Fl::event_x(),Fl::event_y());
		   break;
	}
	return Fl_Gl_Window::handle(event);
}
void GeGlWindow::processMouse(int button, int state, int x, int y) {
    MOOLOG << "GeGlWindow::processMouse called" << std::endl;
    win2int(x,y,_coo[4],_coo[5]);

MOOLOG << "GeGlWindow::processMouse button = " << button << ", state = " << state << std::endl;
    switch (button) {
    case FL_LEFT_MOUSE:
        switch (state) {
        case FL_PUSH:
				_coo[0] = _coo[2] = _coo[4];
				_coo[1] = _coo[3] = _coo[5];
				_state = 1;
				if (_controller) _controller->memHighlightsDown();
            switch(_input_mode) {
            case IM_LINE:
            case IM_CIRCLE:
			case IM_RECT:
                break;
            case IM_NONE:
                if (_controller) {
                    double precision = 5*minPixelRes();
                    MOOLOG << "GeGlWindow:processMouse precision = " << precision << std::endl;
                    //UID uid;
                    _controller->selectByPoint(_coo[4],_coo[5],precision);
                  //_selected = true;
                        //_lastSelUid = uid;
                    //    LOG << "GeGlWindow::processMouse selected = " << uid << std::endl;
                    //}
                }
            }
            break;
        case FL_RELEASE:
        	if ( _input_mode != IM_NONE && _controller )
				if (_controller) _controller->memHighlightsUp();

            switch (_input_mode) {
            case IM_POINT:
                if (_controller) {
                    _controller->addObject(georis::OT_POINT,std::vector<double>(_coo+4,_coo+6));
                    //LOG << "GeGlWindow::processMouse added point, uid = " << uid << std::endl;
                }
                break;
            case IM_LINE:
                if ( sqrt((_coo[2]-_coo[4])*(_coo[2]-_coo[4]) + (_coo[3]-_coo[5])*(_coo[3]-_coo[5])) > 2*minPixelRes())
                    if (_controller) {
                        _controller->addObject(georis::OT_SEGMENT,std::vector<double>(_coo+2,_coo+6));
                        //LOG << "GeGlWindow::processMouse added line, uid = " << uid << std::endl;
                    }

                _coo[2] = _coo[0];
                _coo[3] = _coo[1];
                break;
            case IM_CIRCLE:
                _coo[6] = sqrt((_coo[2]-_coo[4])*(_coo[2]-_coo[4]) + (_coo[3]-_coo[5])*(_coo[3]-_coo[5]));
                if ( _coo[6] > 2*minPixelRes())
                    if (_controller) {
						_coo[4] = _coo[2];_coo[5] = _coo[3];
                        _controller->addObject(georis::OT_CIRCLE,std::vector<double>(_coo+4,_coo+7));
                        //LOG << "GeGlWindow::processMouse added circle, uid = " << uid << std::endl;
                    }
                _coo[2] = _coo[0];
                _coo[3] = _coo[1];
                break;
			case IM_RECT:
				if ( sqrt((_coo[2]-_coo[4])*(_coo[2]-_coo[4]) + (_coo[3]-_coo[5])*(_coo[3]-_coo[5])) > 2*minPixelRes())
                    if (_controller) {
						std::vector<double> tmp(4);
						tmp[0] = *(_coo+2); tmp[1] = *(_coo+3);tmp[2] = *(_coo+2); tmp[3] = *(_coo+5);
                        _controller->addObject(georis::OT_SEGMENT,tmp);
						tmp[0] = *(_coo+2); tmp[1] = *(_coo+5);tmp[2] = *(_coo+4); tmp[3] = *(_coo+5);
                        _controller->addObject(georis::OT_SEGMENT,tmp);
  						tmp[0] = *(_coo+4); tmp[1] = *(_coo+5);tmp[2] = *(_coo+4); tmp[3] = *(_coo+3);
                        _controller->addObject(georis::OT_SEGMENT,tmp);
						tmp[0] = *(_coo+4); tmp[1] = *(_coo+3);tmp[2] = *(_coo+2); tmp[3] = *(_coo+3);
                        _controller->addObject(georis::OT_SEGMENT,tmp);
                    }

                _coo[2] = _coo[0];
                _coo[3] = _coo[1];
                break;
			case IM_NONE:
				if ( _state && _controller && sqrt((_coo[2]-_coo[4])*(_coo[2]-_coo[4]) + (_coo[3]-_coo[5])*(_coo[3]-_coo[5])) > 2*minPixelRes() )
                    _controller->selectByRect( _coo[2],_coo[3],_coo[4],_coo[5]);
            }
            _state = 0;
            //if ( _input_mode != IM_NONE) _input_mode = IM_NONE;
        }
        break;

    case FL_MIDDLE_MOUSE:
        break;
    case 4: // scroll forward
		zoomOut(_coo[4],_coo[5]);
        break;
	 case 5:
		zoomIn(_coo[4],_coo[5]);
        break;

    default:
        ;
    }
    redraw();
}
void GeGlWindow::processDrag(int x,int y){
//	LOG << "GeGlWindow::processDrag called" << std::endl;

   win2int(x,y,_coo[4],_coo[5]);
   if ( _controller ) {
		_controller->highlightObj(_coo[4],_coo[5],3*minPixelRes());
		_controller->moveSelected(_coo[4] - _coo[0],_coo[5] - _coo[1]);
   }
   _coo[0] = _coo[4];
	_coo[1] = _coo[5];
	redraw();
}
void GeGlWindow::processMove(int x,int y){
//	LOG << "GeGlWindow::processMove called" << std::endl;
   win2int(x,y,_coo[4],_coo[5]);
   if ( _controller ) {
		_controller->highlightObj(_coo[4],_coo[5],3*minPixelRes());
   }
   redraw();
}

void GeGlWindow::zoomOut(double px,double py) {
    _left = 1/_zoomFactor*_left + px*(1 -1/_zoomFactor);
    _right = 1/_zoomFactor*_right + px*(1 - 1/_zoomFactor);
    _bottom  = 1/_zoomFactor*_bottom + py*(1-1/_zoomFactor);
    _top = 1/_zoomFactor*_top + py*(1-1/_zoomFactor);
}
void GeGlWindow::zoomIn(double px,double py) {
    _left = _zoomFactor*_left + px*(1 -_zoomFactor);
    _right = _zoomFactor*_right + px*(1 - _zoomFactor);
    _bottom  = _zoomFactor*_bottom + py*(1-_zoomFactor);
    _top = _zoomFactor*_top + py*(1-_zoomFactor);
}


void GeGlWindow::drawPoint(double x,double y,unsigned status) {
    setColor(status);
    glPointSize(5.0f);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glBegin(GL_POINTS);
    glVertex2d(x,y);
    glEnd();
    glDisable(GL_POINT_SMOOTH);
}
void GeGlWindow::drawLine(double x1, double y1, double x2, double y2,unsigned status ) {
    setColor(status);
    glLineWidth(2.1f);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_LINES);
    glVertex2d(x1,y1);
    glVertex2d(x2,y2);
    glEnd();
}
void GeGlWindow::drawCircle(double px, double py, const double r,unsigned status) {
    setColor(status);
    int num_segments = 100;

    float theta = 2 * 3.1415926 / float(num_segments);
    float tangetial_factor = tanf(theta);//calculate the tangential factor

    float radial_factor = cosf(theta);//calculate the radial factor

    float x = r;//we start at angle = 0

    float y = 0;

    glLineWidth(2.1);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glBegin(GL_LINE_LOOP);
    for(int ii = 0; ii < num_segments; ii++) 	{
        glVertex2f(x + px, y + py);//output vertex

        //calculate the tangential vector
        //remember, the radial vector is (x, y)
        //to get the tangential vector we flip those coordinates and negate one of them

        float tx = -y;
        float ty = x;

        //add the tangential vector

        x += tx * tangetial_factor;
        y += ty * tangetial_factor;

        //correct using the radial factor

        x *= radial_factor;
        y *= radial_factor;
    }
    glEnd();

}
void GeGlWindow::setColor(unsigned status) {
	if (status  & MODE_HIGHLIGHTED ){
		glColor3f( 1, 0.5, 0 );
		return;
	}
	if (status & MODE_SELECTED ){
		glColor3f( 0.07, 0.6, 0.95 );
		return;
	}
	if ( status & MODE_FIXED ){
		glColor3f( 0,0,0 );
		return;
	}
   glColor3f( 0, 0, 1 );
}
void GeGlWindow::clearBG() {
    glClearColor(0.8,0.8 ,0.8 ,0);
    glClear(GL_COLOR_BUFFER_BIT);
}
void GeGlWindow::showGrid() {
    int ma = 5;
    int mi = -5;
    glPointSize(1);

    glPushAttrib(GL_ENABLE_BIT);
    glEnable (GL_LINE_SMOOTH);
    glLineWidth(0.1);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x00FF);
    glColor3f(0.7,0.7,0.7);
    glBegin(GL_LINES);
    for (int k=mi; k<=ma; ++k) {
        GLfloat v0[]= {0.1f*k,0.1f*mi};
        GLfloat v1[]= {0.1f*k,0.1f*ma};
        GLfloat v2[]= {0.1f*mi,0.1f*k};
        GLfloat v3[]= {0.1f*ma,0.1f*k};

        glVertex2fv(v0);
        glVertex2fv(v1);
        glVertex2fv(v2);
        glVertex2fv(v3);
    }
    glEnd();
    glDisable(GL_LINE_STIPPLE);
    glPopAttrib();

    glColor3f(0.3,0.3,0.3);
    glBegin(GL_LINES);
    GLfloat v0[]= {0,0.1f*mi};
    GLfloat v1[]= {0,0.1f*ma};
    GLfloat v2[]= {0.1f*mi,0};
    GLfloat v3[]= {0.1f*ma,0};
    glVertex2fv(v0);
    glVertex2fv(v1);
    glVertex2fv(v2);
    glVertex2fv(v3);

    glEnd();
}
void GeGlWindow::draw() {
   //LOG << "GeGlWindow::draw() called" << std::endl;
    setZoom();
    clearBG();
    showGrid();
    showMode();
    if (_controller) _controller->updateView();

    if (glGetError() == GL_NO_ERROR) {
        glFlush();
    }
}
void GeGlWindow::setZoom() {
    glLoadIdentity();
    glOrtho(_left,_right,_bottom,_top,-1.0,1.0);
    glMatrixMode(GL_MODELVIEW);
}
void GeGlWindow::win2int(int wx,int wy,double &ix,double &iy){
    ix = _left + wx*(_right - _left)/w();
    iy = _top - wy*(_top - _bottom)/h();
}
void GeGlWindow::showMode(){
	   if (_input_mode != IM_NONE && _state) {
        switch (_input_mode) {
        case IM_LINE:
            drawLine(_coo[2],_coo[3],_coo[0],_coo[1],MODE_NORMAL);
            break;
		case IM_RECT:
			drawLine(_coo[2],_coo[3],_coo[2],_coo[5],MODE_NORMAL);
			drawLine(_coo[2],_coo[3],_coo[4],_coo[3],MODE_NORMAL);
			drawLine(_coo[4],_coo[3],_coo[4],_coo[5],MODE_NORMAL);
			drawLine(_coo[2],_coo[5],_coo[4],_coo[5],MODE_NORMAL);
			break;
        case IM_CIRCLE:
            _coo[6] = sqrt((_coo[2] - _coo[0])*(_coo[2] - _coo[0]) + (_coo[3] - _coo[1])*(_coo[3] - _coo[1]));
            drawCircle(_coo[2],_coo[3],_coo[6],MODE_NORMAL);
            break;
        }


    }
}

#include "GeGlWindow.h"
#include <FL/gl.h>
#include <FL/Fl.H>
#include "georis.h"
#include <cmath>
#include <iostream>

#include "mooLog.h"

GeGlWindow::GeGlWindow(int x,int y, int w,int h,const char *name):Fl_Gl_Window(x,y,w,h,name) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//ctor
    if ( w <= 0 ) throw std::invalid_argument("GeGlWindow::bad width");
    _left = -1; _right = 1; _top = static_cast<float>(h)/w; _bottom = -static_cast<float>(h)/w;
    glViewport(0,0,w,h);
    memset( _coo,0,sizeof(_coo) );

    _zoomFactor = 0.9;
    _input_mode = georis::IM_NONE;
    _state = 0;
    _controller = nullptr;
    _statusbar = nullptr;
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
        //MOOLOG << "GeGlWindow::handle Mouse Pushed x = " << Fl::event_x() << " y = " << Fl::event_y() << std::endl;
        processMouse(Fl::event_button(),FL_PUSH,Fl::event_x(),Fl::event_y());
        return 1;
    case FL_RELEASE:
        //MOOLOG << "GeGlWindow::handle Mouse Released x = " << Fl::event_x() << " y = " << Fl::event_y() << std::endl;
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
    //MOOLOG << "GeGlWindow::processMouse called x " << x << " y " << y<< std::endl;
    double xCur,yCur;
    win2int(x,y,xCur,yCur);

    //MOOLOG << "GeGlWindow::processMouse button = " << button << ", state = " << state << std::endl;
    switch (button) {
    case FL_LEFT_MOUSE:
        switch (state) {
        case FL_PUSH:
            if (_controller)
                _controller->memHighlightsDown();

            switch(_input_mode) {
            case georis::IM_LINE:
            case georis::IM_CIRCLE:
            case georis::IM_RECT:
                _coo[0] = _coo[2] = xCur;
                _coo[1] = _coo[3] = yCur;
                _state = 1;
                break;
            case georis::IM_ARC:{
                switch ( _state ){
                case 0:
                    _coo[0] = _coo[2] = xCur;
                    _coo[1] = _coo[3] = yCur;
                    _state = 1;
                    break;
                }
                break;
            }
            case georis::IM_NONE:
                _coo[0] = _coo[2] = xCur;
                _coo[1] = _coo[3] = yCur;
                if (_controller) {
                    double precision = 5*minPixelRes();
                    //MOOLOG << "GeGlWindow:processMouse precision = " << precision << std::endl;
                    if ( _controller->selectByPoint(xCur,yCur,precision) == 0 )
                        _state = 1;
                }

            }
            break;
        case FL_RELEASE:
            if ( _input_mode != georis::IM_NONE && _controller )
                if (_controller) _controller->memHighlightsUp();

            switch (_input_mode) {
            case georis::IM_POINT:
                if (_controller) {
                    _controller->addObject(georis::OT_POINT,{xCur,yCur});
                    //LOG << "GeGlWindow::processMouse added point, uid = " << uid << std::endl;
                }
                break;
            case georis::IM_LINE:
                if ( sqrt((_coo[2] - xCur)*(_coo[2] - xCur) + (_coo[3] - yCur)*(_coo[3] - yCur)) > 2*minPixelRes())
                    if (_controller) {
                        _controller->addObject(georis::OT_SEGMENT,{_coo[2],_coo[3],xCur,yCur});
                        //LOG << "GeGlWindow::processMouse added line, uid = " << uid << std::endl;
                    }
                _state = 0;
                //_coo[2] = _coo[0];
                //_coo[3] = _coo[1];
                break;
            case georis::IM_CIRCLE:{
                double r  = sqrt((_coo[2] - xCur)*(_coo[2] - xCur) + (_coo[3] - yCur)*(_coo[3] - yCur));
                if ( r > 2*minPixelRes())
                    if ( _controller != nullptr ) {
                        _coo[4] = _coo[2];_coo[5] = _coo[3];
                        _controller->addObject(georis::OT_CIRCLE,{_coo[2],_coo[3],r});
                        //LOG << "GeGlWindow::processMouse added circle, uid = " << uid << std::endl;
                    }
                _state = 0;
                break;
            }
            case georis::IM_ARC:{
                switch (_state){
                case 1:
                    _coo[4] = xCur;
                    _coo[5] = yCur;
                    _state = 2;
                    break;
                case 3:{
                    if ( _controller != nullptr ) {
                        if ( _coo[7] > 0 ) {
                            _controller->addObject(georis::OT_ARC,{_coo[2],_coo[3],_coo[4],_coo[5], _coo[0],_coo[1]});
                        }
                        else{
                            _controller->addObject(georis::OT_ARC,{_coo[2],_coo[3], _coo[0],_coo[1],_coo[4],_coo[5]});
                        }
                    }
                    _state = 0;
                    break;
                }
                }
                break;
            }
            case georis::IM_RECT:{
                if ( sqrt((_coo[2] - xCur)*(_coo[2] - xCur) + (_coo[3] - yCur)*(_coo[3] - yCur)) > 2*minPixelRes())
                    if (_controller) {
                        std::vector<double> tmp(4);
                        tmp[0] = _coo[2]; tmp[1] = _coo[3];tmp[2] = _coo[2]; tmp[3] = yCur;
                        _controller->addObject(georis::OT_SEGMENT,tmp);                        
                        tmp[0] = _coo[2]; tmp[1] = _coo[3];tmp[2] = xCur; tmp[3] = _coo[3];
                        _controller->addObject(georis::OT_SEGMENT,tmp);
                        tmp[0] = xCur; tmp[1] = yCur;tmp[2] = xCur; tmp[3] = _coo[3];
                        _controller->addObject(georis::OT_SEGMENT,tmp);
                        tmp[0] = xCur; tmp[1] = yCur;tmp[2] = _coo[2]; tmp[3] = yCur;
                        _controller->addObject(georis::OT_SEGMENT,tmp);
                    }
                _state = 0;
                break;
            }
            case georis::IM_NONE:
                if ( (_controller != nullptr) && (std::sqrt((_coo[2]-xCur)*(_coo[2]-xCur) + (_coo[3]-yCur)*(_coo[3]-yCur)) > 2*minPixelRes()) )
                    _controller->selectByRect( _coo[2],_coo[3],xCur,yCur);
                _state = 0;
            }
        }
        break;

    case FL_MIDDLE_MOUSE:
        break;
    case 4: // scroll forward
        zoomOut(xCur,yCur);
        break;
    case 5:
        zoomIn(xCur,yCur);
        break;

    default:
        ;
    }
    redraw();
}
void GeGlWindow::processDrag(int x,int y){
    //MOOLOG << "GeGlWindow::processDrag called" << std::endl;
    double xCur,yCur;
    win2int(x,y,xCur,yCur);
    if ( _controller ) {
        _controller->highlightObj(xCur,yCur,3*minPixelRes());
        _controller->moveSelected((xCur - _coo[0]),(yCur - _coo[1]));
    }
    // Update current pos
    _coo[0] = xCur;
    _coo[1] = yCur;
    redraw();
}

void GeGlWindow::processMove(int x,int y){
    //MOOLOG << "GeGlWindow::processMove x "<< x << " y "<< y << std::endl;
    double xCur,yCur;
    win2int(x,y,xCur,yCur);
    if ( _controller ) {
        _controller->highlightObj(xCur,yCur,3*minPixelRes());
    }
    // Update current pos
    _coo[0] = xCur;
    _coo[1] = yCur;

    char buffer[64];
    snprintf(buffer,sizeof(buffer),"%6.3f, %6.3f",xCur,yCur);

    if ( _statusbar != nullptr )
        _statusbar->copy_label(buffer);

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
    setStyle(status);
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
    setStyle(status);
    glBegin(GL_LINES);
    glVertex2d(x1,y1);
    glVertex2d(x2,y2);
    glEnd();
}
void GeGlWindow::drawDimLine(double x1, double y1,
                             double x2, double y2,
                             double x3, double y3,
                             unsigned status ) {
    setColor(status);
    glBegin(GL_LINES);
    glVertex2d(x1,y1);
    glVertex2d(x2,y2);
    glEnd();
    glBegin(GL_LINES);
    glVertex2d(x3,y3);
    glVertex2d(x3 + x2 - x1,y3 + y2 - y1);
    glEnd();
}
void GeGlWindow::drawDimLine(double xc, double yc, double r, double x2, double y2,unsigned status){
    setColor(status);
    glBegin(GL_LINES);
    glVertex2d(xc,yc);
    glVertex2d(x2,y2);
    glVertex2d(x2 +40 ,y2);
    glEnd();
}

void GeGlWindow::drawCircle(double px, double py, double r,unsigned status) {
    setColor(status);
    setStyle(status);

    const int num_segments = 100;

    float theta = 2 * 3.1415926 / float(num_segments);
    float tangetial_factor = tanf(theta);//calculate the tangential factor

    float radial_factor = cosf(theta);//calculate the radial factor

    float x = r;//we start at angle = 0

    float y = 0;

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

///
/// \brief GeGlWindow::drawArc
/// \param cx
/// \param cy
/// \param bx
/// \param by
/// \param ex
/// \param ey
/// \param status
/// Draws arc with  center (cx,cy) from (bx,by) CCW to (ex,ey)
///
void GeGlWindow::drawArc(double cx, double cy, double bx,double by, double ex, double ey,unsigned status){    
    setColor(status);
    setStyle(status);

    const int num_segments = 100;

    double dxb = bx - cx;
    double dyb = by - cy;
    double phib = atan2(dyb,dxb);

    double dxe = ex - cx;
    double dye = ey - cy;
    double phie = atan2(dye,dxe);

    double cro = dxb*dye - dxe*dyb;

    double angle = phie - phib;

    if ( cro > 0 ){
        if ( phie < phib )
            angle += 2*M_PI;
    }
    else if ( cro < 0 ){
        if ( phie < phib )
            angle += 2*M_PI;
    }

    float theta = static_cast<float>(angle) / static_cast<float>(num_segments);

    float tangetial_factor = tanf(theta);//calculate the tangential factor

    float radial_factor = cosf(theta);//calculate the radial factor

    float x = bx - cx;
    float y = by - cy;



    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glBegin(GL_LINE_STRIP);
    for(int ii = 0; ii < num_segments; ii++) 	{
        glVertex2f(cx + x, cy + y);//output vertex

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
    glVertex2f(static_cast<float>(ex),static_cast<float>(ey));
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
void GeGlWindow::setStyle(unsigned status) {
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (status  & MODE_CONSTRUCTI ){
        glLineWidth(0.5f);
        //glLineStipple(1, 0x3F07);
        glLineStipple(3, 0xFF88);
        glEnable(GL_LINE_STIPPLE);

    }
    else{
        glLineWidth(2.1f);
        glLineStipple(1,0xFFFF);
        glDisable(GL_LINE_STIPPLE);
    }
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
    if (_state) {
        switch (_input_mode) {
        case georis::IM_LINE:
            drawLine(_coo[2],_coo[3],_coo[0],_coo[1],MODE_NORMAL);
            break;
        case georis::IM_RECT:
            drawLine(_coo[0],_coo[1],_coo[0],_coo[3],MODE_NORMAL);
            drawLine(_coo[0],_coo[1],_coo[2],_coo[1],MODE_NORMAL);
            drawLine(_coo[2],_coo[3],_coo[2],_coo[1],MODE_NORMAL);
            drawLine(_coo[2],_coo[3],_coo[0],_coo[3],MODE_NORMAL);
            break;
        case georis::IM_CIRCLE:
            _coo[6] = sqrt((_coo[2] - _coo[0])*(_coo[2] - _coo[0]) + (_coo[3] - _coo[1])*(_coo[3] - _coo[1]));
            drawCircle(_coo[2],_coo[3],_coo[6],MODE_NORMAL);
            break;
        case georis::IM_ARC:{
            switch (_state){
            case 1:
                drawLine(_coo[2],_coo[3],_coo[0],_coo[1],MODE_CONSTRUCTI);
                _coo[6] = sqrt((_coo[2] - _coo[0])*(_coo[2] - _coo[0]) + (_coo[3] - _coo[1])*(_coo[3] - _coo[1]));
                drawCircle(_coo[2],_coo[3],_coo[6],MODE_CONSTRUCTI);
                break;
            case 2:{
                double r = sqrt((_coo[4] - _coo[0])*(_coo[4] - _coo[0]) + (_coo[5] - _coo[1])*(_coo[5] - _coo[1]));
                if ( r > 100*minPixelRes() ){
                    // Remember direction to draw an arc
                    double x1c = _coo[4] - _coo[2];
                    double y1c = _coo[5] - _coo[3];
                    double xcurc = _coo[0] - _coo[2];
                    double ycurc = _coo[1] - _coo[3];
                    _coo[7] = x1c*ycurc - xcurc*y1c;
/*
MOOLOG << "==================================================================" << std::endl;
MOOLOG << "minPixelRes() " << minPixelRes() << std::endl;
MOOLOG << "GeGlWindow::showMode arc state 2 coo7   " << _coo[7] << " r = " << r << std::endl;
MOOLOG << "==================================================================" << std::endl;
*/
                    _state = 3;
                }
                break;
            }
            case 3:{
                double r = sqrt((_coo[4] - _coo[2])*(_coo[4] - _coo[2]) + (_coo[5] - _coo[3])*(_coo[5] - _coo[3]));
                double rcurc = sqrt((_coo[0] - _coo[2])*(_coo[0] - _coo[2]) + (_coo[1] - _coo[3])*(_coo[1] - _coo[3]));

                double xcurc = _coo[0] - _coo[2];
                double ycurc = _coo[1] - _coo[3];

                // Normalize endvec
                xcurc *= (r/rcurc);
                ycurc *= (r/rcurc);
                _coo[0] = _coo[2] + xcurc;
                _coo[1] = _coo[3] + ycurc;

                if ( rcurc > 2*minPixelRes() ){
                    if ( _coo[7] > 0 ) // CCW
                        drawArc(_coo[2],_coo[3], _coo[4],_coo[5], _coo[0],_coo[1] ,MODE_CONSTRUCTI);
                    else
                        drawArc(_coo[2],_coo[3], _coo[0],_coo[1], _coo[4],_coo[5], MODE_CONSTRUCTI);


                    drawLine(_coo[2],_coo[3],_coo[0],_coo[1],MODE_CONSTRUCTI);
                    //drawLine(_coo[2],_coo[3],_coo[4],_coo[5],MODE_CONSTRUCTI);
                    drawLine(_coo[2],_coo[3],_coo[4],_coo[5],MODE_NORMAL);
                }
            }
            }
            break;
        }
        case georis::IM_NONE:{
            drawLine(_coo[0],_coo[1],_coo[0],_coo[3],MODE_CONSTRUCTI);
            drawLine(_coo[0],_coo[1],_coo[2],_coo[1],MODE_CONSTRUCTI);
            drawLine(_coo[2],_coo[3],_coo[2],_coo[1],MODE_CONSTRUCTI);
            drawLine(_coo[2],_coo[3],_coo[0],_coo[3],MODE_CONSTRUCTI);
        }
        }
    }
}

#include "GeGlWindow.h"
#include <FL/gl.h>
#include <FL/Fl.H>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <cmath>
#include <iostream>

#include "georis.h"
#include "common_math.h"

#include "mooLog.h"

GeGlWindow::GeGlWindow(int x,int y, int w,int h,const char *name):Fl_Gl_Window(x,y,w,h,name) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_left = -1; m_right = 1; m_top = static_cast<float>(h)/w; m_bottom = -static_cast<float>(h)/w;
    m_currentZoom = 1.0;
    glViewport(0,0,w,h);

    memset( m_coo,0,sizeof(m_coo) );

    m_input_mode = georis::IM_NONE;
    m_state = 0;
    m_controller = nullptr;

    m_fontdata.init("//usr//share//fonts//TTF//DejaVuSans.ttf", 14);
}

void GeGlWindow::resize(int x,int y, int ww,int hh){
    float wdelta = (ww*(m_right - m_left)/w() - (m_right-m_left))/2;
    float hdelta = (hh*(m_top - m_bottom)/h() - (m_top - m_bottom))/2;
    m_left -=  wdelta;
    m_right += wdelta;
    m_top +=  hdelta;
    m_bottom -=  hdelta;
    glViewport(0,0,ww,hh);
    Fl_Gl_Window::resize(x,y,ww,hh);
}
int GeGlWindow::handle(int event){
    switch (event){
    case FL_PUSH:
        //MOOLOG << "GeGlWindow::handle Mouse Pushed x = " << Fl::event_x() << " y = " << Fl::event_y() << std::endl;
        if (Fl::event_clicks() > 0) {
            MOOLOG << "GeGlWindow::handle Mouse double click Pushed x = " << Fl::event_x() << " y = " << Fl::event_y() << std::endl;
        }
        else{
            processMouse(Fl::event_button(),FL_PUSH,Fl::event_x(),Fl::event_y());
        }
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
    double xCur,yCur;
    win2int(x,y,xCur,yCur);

    MOOLOG << "GeGlWindow::processMouse called xCur " << xCur << " yCur " << yCur << std::endl;

    //MOOLOG << "GeGlWindow::processMouse button = " << button << ", state = " << state << std::endl;
    switch (button) {
    case FL_LEFT_MOUSE:
        switch (state) {
        case FL_PUSH:
            if (m_controller)
                m_controller->memHighlightsDown(xCur,yCur);

            switch(m_input_mode) {
            case georis::IM_LINE:
            case georis::IM_RAY:
            case georis::IM_SEGMENT:
            case georis::IM_CIRCLE:
            case georis::IM_RECT:
                m_coo[0] = m_coo[2] = xCur;
                m_coo[1] = m_coo[3] = yCur;
                m_state = 1;
                break;
            case georis::IM_ARC:{
                switch ( m_state ){
                case 0:
                    m_coo[0] = m_coo[2] = xCur;
                    m_coo[1] = m_coo[3] = yCur;
                    m_state = 1;
                    break;
                }
                break;
            }
            case georis::IM_NONE:
                m_coo[0] = m_coo[2] = xCur;
                m_coo[1] = m_coo[3] = yCur;
                if (m_controller) {
                    double precision = 5*minPixelRes();
                    //MOOLOG << "GeGlWindow:processMouse precision = " << precision << std::endl;
                    if ( m_controller->selectByPoint(xCur,yCur,precision) == 0 )
                        m_state = 1;
                }

            }
            break;
        case FL_RELEASE:
            if ( m_input_mode != georis::IM_NONE && m_controller )
                if (m_controller) m_controller->memHighlightsUp(xCur,yCur);

            switch (m_input_mode) {
            case georis::IM_POINT:
                if (m_controller) {
                    m_controller->addObject(georis::OT_POINT,{xCur,yCur});
                    //LOG << "GeGlWindow::processMouse added point, uid = " << uid << std::endl;
                }
                break;
            case georis::IM_LINE:
                if ( sqrt((m_coo[2] - xCur)*(m_coo[2] - xCur) + (m_coo[3] - yCur)*(m_coo[3] - yCur)) > 2*minPixelRes())
                    if (m_controller) {
                        m_controller->addObject(georis::OT_LINE,{m_coo[2],m_coo[3],xCur,yCur});
                        //LOG << "GeGlWindow::processMouse added line, uid = " << uid << std::endl;
                    }
                m_state = 0;
                break;
            case georis::IM_RAY:
                if ( sqrt((m_coo[2] - xCur)*(m_coo[2] - xCur) + (m_coo[3] - yCur)*(m_coo[3] - yCur)) > 2*minPixelRes())
                    if (m_controller) {
                        m_controller->addObject(georis::OT_RAY,{m_coo[2],m_coo[3],xCur,yCur});
                        //LOG << "GeGlWindow::processMouse added ray, uid = " << uid << std::endl;
                    }
                m_state = 0;
                break;
            case georis::IM_SEGMENT:
                if ( sqrt((m_coo[2] - xCur)*(m_coo[2] - xCur) + (m_coo[3] - yCur)*(m_coo[3] - yCur)) > 2*minPixelRes())
                    if (m_controller) {
                        m_controller->addObject(georis::OT_SEGMENT,{m_coo[2],m_coo[3],xCur,yCur});
                        //LOG << "GeGlWindow::processMouse added segment, uid = " << uid << std::endl;
                    }
                m_state = 0;
                break;
            case georis::IM_CIRCLE:{
                double r  = sqrt((m_coo[2] - xCur)*(m_coo[2] - xCur) + (m_coo[3] - yCur)*(m_coo[3] - yCur));
                if ( r > 2*minPixelRes())
                    if ( m_controller != nullptr ) {
                        m_coo[4] = m_coo[2];m_coo[5] = m_coo[3];
                        m_controller->addObject(georis::OT_CIRCLE,{m_coo[2],m_coo[3],r});
                        //LOG << "GeGlWindow::processMouse added circle, uid = " << uid << std::endl;
                    }
                m_state = 0;
                break;
            }
            case georis::IM_ARC:{
                switch (m_state){
                case 1:
                    m_coo[4] = xCur;
                    m_coo[5] = yCur;
                    m_state = 2;
                    break;
                case 3:{
                    if ( m_controller != nullptr ) {
                        if ( m_coo[7] > 0 ) {
                            m_controller->addObject(georis::OT_ARC,{m_coo[2],m_coo[3],m_coo[4],m_coo[5], m_coo[0],m_coo[1]});
                        }
                        else{
                            m_controller->addObject(georis::OT_ARC,{m_coo[2],m_coo[3], m_coo[0],m_coo[1],m_coo[4],m_coo[5]});
                        }
                    }
                    m_state = 0;
                    break;
                }
                }
                break;
            }
            case georis::IM_RECT:{
                if ( sqrt((m_coo[2] - xCur)*(m_coo[2] - xCur) + (m_coo[3] - yCur)*(m_coo[3] - yCur)) > 2*minPixelRes())
                    if (m_controller) {
                        std::vector<double> tmp(4);
                        tmp[0] = m_coo[2]; tmp[1] = m_coo[3];tmp[2] = m_coo[2]; tmp[3] = yCur;
                        m_controller->addObject(georis::OT_SEGMENT,tmp);
                        tmp[0] = m_coo[2]; tmp[1] = m_coo[3];tmp[2] = xCur; tmp[3] = m_coo[3];
                        m_controller->addObject(georis::OT_SEGMENT,tmp);
                        tmp[0] = xCur; tmp[1] = yCur;tmp[2] = xCur; tmp[3] = m_coo[3];
                        m_controller->addObject(georis::OT_SEGMENT,tmp);
                        tmp[0] = xCur; tmp[1] = yCur;tmp[2] = m_coo[2]; tmp[3] = yCur;
                        m_controller->addObject(georis::OT_SEGMENT,tmp);
                    }
                m_state = 0;
                break;
            }
            case georis::IM_NONE:
                if ( (m_controller != nullptr) && (std::sqrt((m_coo[2]-xCur)*(m_coo[2]-xCur) + (m_coo[3]-yCur)*(m_coo[3]-yCur)) > 2*minPixelRes()) )
                    m_controller->selectByRect( m_coo[2],m_coo[3],xCur,yCur);
                m_state = 0;
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
    double xCur,yCur;
    win2int(x,y,xCur,yCur);
    MOOLOG << "GeGlWindow::processDrag called xCur "<< xCur << " yCur " << yCur << std::endl;

    if ( m_controller ) {
        m_controller->highlightObj(xCur,yCur,3*minPixelRes());
        //m_controller->moveSelected((xCur - m_coo[0]),(yCur - m_coo[1]));
        m_controller->processDrag(xCur,yCur);

    }
    updateCurrentPos(xCur,yCur);
    redraw();
}
void GeGlWindow::updateCurrentPos(double xCur, double yCur){
    m_coo[0] = xCur;
    m_coo[1] = yCur;/*
    if ( m_statusbar != nullptr ){
        char buffer[64];
        snprintf(buffer,sizeof(buffer),"%6.3f, %6.3f",xCur,yCur);
        m_statusbar->copy_label(buffer);
    }
    */
}
void GeGlWindow::processMove(int x,int y){
    //MOOLOG << "GeGlWindow::processMove x "<< x << " y "<< y << std::endl;
    double xCur,yCur;
    win2int(x,y,xCur,yCur);
    if ( m_controller ) {
        m_controller->highlightObj(xCur,yCur,3*minPixelRes());
    }
    updateCurrentPos(xCur,yCur);
    redraw();
}

void GeGlWindow::zoomOut(double px,double py) {
    m_currentZoom /= m_zoomFactor;
    m_left = 1/m_zoomFactor*m_left + px*(1 -1/m_zoomFactor);
    m_right = 1/m_zoomFactor*m_right + px*(1 - 1/m_zoomFactor);
    m_bottom  = 1/m_zoomFactor*m_bottom + py*(1-1/m_zoomFactor);
    m_top = 1/m_zoomFactor*m_top + py*(1-1/m_zoomFactor);
}
void GeGlWindow::zoomIn(double px,double py) {
    m_currentZoom *= m_zoomFactor;
    m_left = m_zoomFactor*m_left + px*(1 -m_zoomFactor);
    m_right = m_zoomFactor*m_right + px*(1 - m_zoomFactor);
    m_bottom  = m_zoomFactor*m_bottom + py*(1-m_zoomFactor);
    m_top = m_zoomFactor*m_top + py*(1-m_zoomFactor);
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
void GeGlWindow::drawSegment(double x1, double y1, double x2, double y2,unsigned status ) {
    setColor(status);
    setStyle(status);
    glBegin(GL_LINES);
    glVertex2d(x1,y1);
    glVertex2d(x2,y2);
    glEnd();
}
void GeGlWindow::drawRay(double x1, double y1, double x2, double y2,unsigned status ) {
    setColor(status);
    setStyle(status);
    georis::AABBr scree = getScreenBox();
    double x2_ = x2, y2_ = y2;
    if ( scree.contains({x2,y2}) ){
        if ( x1 < x2 ){
            // find intersection with right border
            x2_ = scree.TR.x;
            y2_ = y1 + (y2-y1)*(x2_ - x1)/(x2-x1);
        }
        else if ( x2 < x1 ){
            // find intersection with left border
            x2_ = scree.BL.x;
            y2_ = y1 + (y2-y1)*(x2_ - x1)/(x2-x1);
        }
        else{
            if ( y1 < y2 )
                y2_ = scree.TR.y;
            else if ( y2 < y1 )
                y2_ = scree.BL.y;
        }
    }
    glBegin(GL_LINES);
    glVertex2d(x1,y1);
    glVertex2d(x2_,y2_);
    glEnd();

}
void GeGlWindow::drawLine(double x1, double y1, double x2, double y2,unsigned status ) {
    setColor(status);
    setStyle(status);
    georis::AABBr scree = getScreenBox();
    double x1_ = x1, y1_ = y1, x2_ = x2, y2_ = y2;

    if ( scree.contains({x1,y1}) ){
        if ( x1 < x2 ){
            // find intersection with right border
            x1_ = scree.BL.x;
            y1_ = y1 + (y2-y1)*(x1_ - x1)/(x2-x1);
        }
        else if ( x2 < x1 ){
            // find intersection with right border
            x1_ = scree.TR.x;
            y1_ = y1 + (y2-y1)*(x1_ - x1)/(x2-x1);
        }
        else{
            if ( y1 < y2 )
                y1_ = scree.BL.y;
            else if ( y2 < y1 )
                y1_ = scree.TR.y;
        }
    }
    if ( scree.contains({x2,y2}) ){
        if ( x1 < x2 ){
            // find intersection with right border
            x2_ = scree.TR.x;
            y2_ = y1 + (y2-y1)*(x2_ - x1)/(x2-x1);
        }
        else if ( x2 < x1 ){
            // find intersection with left border
            x2_ = scree.BL.x;
            y2_ = y1 + (y2-y1)*(x2_ - x1)/(x2-x1);
        }
        else{
            if ( y1 < y2 )
                y2_ = scree.TR.y;
            else if ( y2 < y1 )
                y2_ = scree.BL.y;
        }
    }
    glBegin(GL_LINES);
    glVertex2d(x1_,y1_);
    glVertex2d(x2_,y2_);
    glEnd();
}
///
/// \brief GeGlWindow::drawDimLine - aligned dim line between two points
/// \param x1 - first point x
/// \param y1 - first point y
/// \param x2 - second point x
/// \param y2 - second point x
/// \param dx - dimstring x position
/// \param dy - dimstring y position
/// \param dimstr
/// \param status
///
void GeGlWindow::drawDimLine(double x1, double y1,
                             double x2, double y2,
                             double dx, double dy,
                             const char* dimstr,
                             unsigned status ) {
    // Draw dimension lines
    setColor(status);
    setStyle(status);

    glBegin(GL_LINES);
    glVertex2d(x1,y1);
    glVertex2d(x1 + 1.1*dy*(y2-y1),y1 + 1.1*dy*(x1-x2));
    glEnd();
    glBegin(GL_LINES);
    glVertex2d(x2,y2);
    glVertex2d(x2 + 1.1*dy*(y2-y1),y2 + 1.1*dy*(x1-x2));
    glEnd();
    glBegin(GL_LINES);
    glVertex2d(x1 + dy*(y2-y1),y1 + dy*(x1-x2));
    glVertex2d(x2 + dy*(y2-y1),y2 + dy*(x1-x2));
    glEnd();
    //Draw dimension string
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(x1 + dx*(x2-x1) + dy*(y2-y1),y1 + dx*(y2-y1) + dy*(x1-x2),0);
    glRotatef(180*atan2f(y2-y1,x2-x1)/M_PI,0,0,1);
    glScalef(m_currentZoom,m_currentZoom,1);
    m_fontdata.render_text(dimstr);
    glPopMatrix();
}
void GeGlWindow::drawRadiusDimLine(double xc,
                             double yc,
                             double r,
                             double dir,
                             double offset,
                             const char* dimstr,
                             unsigned status){
    setColor(status);
    setStyle(status);

    glBegin(GL_LINES);
    glVertex2d(xc,yc);
    if ( offset > 1.0 )
        glVertex2d(xc + offset*r*cosd(dir),yc + offset*r*sind(dir));
    else
        glVertex2d(xc + r*cosd(dir),yc + r*sind(dir));
    glEnd();
    //Draw dimension string
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(xc + offset*r*cosd(dir),yc + offset*r*sind(dir),0);
    glRotatef(dir,0,0,1);
    glScalef(m_currentZoom,m_currentZoom,1);
    m_fontdata.render_text(dimstr);
    glPopMatrix();
}
void GeGlWindow::drawAngleDimLine(double cx, double cy,double bdir, double edir,  double r, double textdir,const char *dimstr,unsigned status) {
    // Draw dimension lines
    setColor(status);
    setStyle(status);

    intDrawArc(cx,cy,cx+r*cosd(bdir), cy + sind(bdir),cx + cosd(edir),cy + sind(edir));
    //Draw dimension string
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(cx + r*cosd((bdir+edir)/2 + 180*textdir),cy + r*sind((bdir+edir)/2 + 180*textdir),0);
    glRotatef((bdir+edir)/2 + 180*textdir-90,0,0,1);
    glScalef(m_currentZoom,m_currentZoom,1);
    m_fontdata.render_text(dimstr);
    glPopMatrix();
}
void GeGlWindow::drawCircle(double px, double py, double r,unsigned status) {
    setColor(status);
    setStyle(status);

    const int num_segments = 100;

    float theta = 2 * 3.1415926 / float(num_segments);
    float tangetial_factor = tanf(theta);//calculate the tangential factor3

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
    intDrawArc(cx,cy,bx,by,ex,ey);
}
void GeGlWindow::intDrawArc(double cx, double cy, double bx, double by, double ex, double ey){
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
    if (status  & MODE_CONSTRAINED ){
        glColor3f( 0, 1.0, 0 );
        return;
    }
    //glColor3f( 0, 0, 1 );
    glColor3ub(0x19,0x68,0xc7);
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
        if ( status & MODE_DIMLINE )
            glLineWidth(0.5f);
        else
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
void GeGlWindow::showInfo(){
    char buffer[64];
    snprintf(buffer,sizeof(buffer),"%6.3f",m_currentZoom );

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(m_left,m_bottom,0);
    glScalef(m_currentZoom,m_currentZoom,1);
    setColor(MODE_NORMAL);
    setStyle(MODE_NORMAL);
    m_fontdata.render_text(buffer,georis::FontData::TJ_LEFT);
    glPopMatrix();
}
void GeGlWindow::draw() {
    //LOG << "GeGlWindow::draw() called" << std::endl;
    setZoom();
    clearBG();
    showGrid();
    showMode();
    if (m_controller) m_controller->updateView();
    showInfo();
    if (glGetError() == GL_NO_ERROR) {
        glFlush();
    }
}
void GeGlWindow::setZoom() {
    glLoadIdentity();
    glOrtho(m_left,m_right,m_bottom,m_top,-1.0,1.0);
    glMatrixMode(GL_MODELVIEW);
}
void GeGlWindow::win2int(int wx,int wy,double &ix,double &iy){
    ix = m_left + wx*(m_right - m_left)/w();
    iy = m_top - wy*(m_top - m_bottom)/h();
}
void GeGlWindow::showMode(){
    if (m_state) {
        switch (m_input_mode) {
        case georis::IM_SEGMENT:
            drawSegment(m_coo[2],m_coo[3],m_coo[0],m_coo[1],MODE_NORMAL);
            break;
        case georis::IM_RAY:
            drawRay(m_coo[2],m_coo[3],m_coo[0],m_coo[1],MODE_NORMAL);
            break;
        case georis::IM_LINE:
            drawLine(m_coo[2],m_coo[3],m_coo[0],m_coo[1],MODE_NORMAL);
            break;
        case georis::IM_RECT:
            drawSegment(m_coo[0],m_coo[1],m_coo[0],m_coo[3],MODE_NORMAL);
            drawSegment(m_coo[0],m_coo[1],m_coo[2],m_coo[1],MODE_NORMAL);
            drawSegment(m_coo[2],m_coo[3],m_coo[2],m_coo[1],MODE_NORMAL);
            drawSegment(m_coo[2],m_coo[3],m_coo[0],m_coo[3],MODE_NORMAL);
            break;
        case georis::IM_CIRCLE:
            m_coo[6] = sqrt((m_coo[2] - m_coo[0])*(m_coo[2] - m_coo[0]) + (m_coo[3] - m_coo[1])*(m_coo[3] - m_coo[1]));
            drawCircle(m_coo[2],m_coo[3],m_coo[6],MODE_NORMAL);
            break;
        case georis::IM_ARC:{
            switch (m_state){
            case 1:
                drawSegment(m_coo[2],m_coo[3],m_coo[0],m_coo[1],MODE_CONSTRUCTI);
                m_coo[6] = sqrt((m_coo[2] - m_coo[0])*(m_coo[2] - m_coo[0]) + (m_coo[3] - m_coo[1])*(m_coo[3] - m_coo[1]));
                drawCircle(m_coo[2],m_coo[3],m_coo[6],MODE_CONSTRUCTI);
                break;
            case 2:{
                double r = sqrt((m_coo[4] - m_coo[0])*(m_coo[4] - m_coo[0]) + (m_coo[5] - m_coo[1])*(m_coo[5] - m_coo[1]));
                if ( r > 100*minPixelRes() ){
                    // Remember direction to draw an arc
                    double x1c = m_coo[4] - m_coo[2];
                    double y1c = m_coo[5] - m_coo[3];
                    double xcurc = m_coo[0] - m_coo[2];
                    double ycurc = m_coo[1] - m_coo[3];
                    m_coo[7] = x1c*ycurc - xcurc*y1c;
/*
MOOLOG << "==================================================================" << std::endl;
MOOLOG << "minPixelRes() " << minPixelRes() << std::endl;
MOOLOG << "GeGlWindow::showMode arc state 2 coo7   " << m_coo[7] << " r = " << r << std::endl;
MOOLOG << "==================================================================" << std::endl;
*/
                    m_state = 3;
                }
                break;
            }
            case 3:{
                double r = sqrt((m_coo[4] - m_coo[2])*(m_coo[4] - m_coo[2]) + (m_coo[5] - m_coo[3])*(m_coo[5] - m_coo[3]));
                double rcurc = sqrt((m_coo[0] - m_coo[2])*(m_coo[0] - m_coo[2]) + (m_coo[1] - m_coo[3])*(m_coo[1] - m_coo[3]));

                double xcurc = m_coo[0] - m_coo[2];
                double ycurc = m_coo[1] - m_coo[3];

                // Normalize endvec
                xcurc *= (r/rcurc);
                ycurc *= (r/rcurc);
                m_coo[0] = m_coo[2] + xcurc;
                m_coo[1] = m_coo[3] + ycurc;

                if ( rcurc > 2*minPixelRes() ){
                    if ( m_coo[7] > 0 ) // CCW
                        drawArc(m_coo[2],m_coo[3], m_coo[4],m_coo[5], m_coo[0],m_coo[1] ,MODE_CONSTRUCTI);
                    else
                        drawArc(m_coo[2],m_coo[3], m_coo[0],m_coo[1], m_coo[4],m_coo[5], MODE_CONSTRUCTI);


                    drawSegment(m_coo[2],m_coo[3],m_coo[0],m_coo[1],MODE_CONSTRUCTI);
                    //drawSegment(m_coo[2],m_coo[3],m_coo[4],m_coo[5],MODE_CONSTRUCTI);
                    drawSegment(m_coo[2],m_coo[3],m_coo[4],m_coo[5],MODE_NORMAL);
                }
            }
            }
            break;
        }
        case georis::IM_NONE:{
            drawSegment(m_coo[0],m_coo[1],m_coo[0],m_coo[3],MODE_CONSTRUCTI);
            drawSegment(m_coo[0],m_coo[1],m_coo[2],m_coo[1],MODE_CONSTRUCTI);
            drawSegment(m_coo[2],m_coo[3],m_coo[2],m_coo[1],MODE_CONSTRUCTI);
            drawSegment(m_coo[2],m_coo[3],m_coo[0],m_coo[3],MODE_CONSTRUCTI);
        }
        }
    }
}
georis::AABBr GeGlWindow::getScreenBox()const{
    georis::AABBr r;
    r.BL.x = m_left;
    r.BL.y = m_bottom;
    r.TR.x = m_right;
    r.TR.y = m_top;
    return r;
}

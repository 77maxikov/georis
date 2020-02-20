#include <stdio.h>
#include <iostream>

#include "GeVisualizer.h"
#include <cmath>

#include "mooLog.h"
GeVisualizer* GeVisualizer::_instance = nullptr;


static inline double GET_ABS(double x) {
    return x>0?x:-x;
}

void GeVisualizer::drawAALine( double x1, double y1, double x2, double y2, //coordinates of the line
                                 float w, //width/thickness of the line in pixel
                                 float Cr, float Cg, float Cb, //RGB color components
                                 float Br, float Bg, float Bb, //color of background when alphablend=false,
                                 //Br=alpha of color when alphablend=true
                                 bool alphablend) { //use alpha blend or not
    double t;
    double R;
    float A;

    if ( alphablend)
        A=Br;
    else
        A=1.0f;


    //determine angle of the line to horizontal
    double tx=0,ty=0; //core thinkness of a line
    double Rx=0,Ry=0; //fading edge of a line
    double dx=y1-y2;
    double dy=x2-x1;

    t = 0.002;
    R = 0.002;
    double L=sqrt(dx*dx+dy*dy);
    dx/=L;
    dy/=L;
    tx=t*dx;
    ty=t*dy;
    Rx=R*dx;
    Ry=R*dy;

    //draw the line by triangle strip
    glBegin(GL_TRIANGLE_STRIP);
    if ( !alphablend) {
        glColor3f( Br,Bg,Bb);
    } else {
        glColor4f( Cr,Cg,Cb, 0);
    }
    glVertex2f( x1-tx-Rx, y1-ty-Ry); //fading edge
    glVertex2f( x2-tx-Rx, y2-ty-Ry);

    if ( !alphablend) {
        glColor3f( Cr,Cg,Cb);
    } else {
        glColor4f( Cr,Cg,Cb, A);
    }
    glVertex2f( x1-tx,y1-ty); //core
    glVertex2f( x2-tx,y2-ty);
    glVertex2f( x1+tx,y1+ty);
    glVertex2f( x2+tx,y2+ty);

    if ( !alphablend) {
        glColor3f( Br,Bg,Bb);
    } else {
        glColor4f( Cr,Cg,Cb, 0);
    }
    glVertex2f( x1+tx+Rx, y1+ty+Ry); //fading edge
    glVertex2f( x2+tx+Rx, y2+ty+Ry);

    glEnd();
}

void GeVisualizer::displayFunc(void) {
    GeVisualizer::instance()->show();
}
void GeVisualizer::keyboardFunc(GLubyte key,GLint x,GLint y) {
    GeVisualizer::instance()->processKeyboard(key,x,y);
}
void GeVisualizer::mouseFunc(int button, int state, int x, int y) {
    GeVisualizer::instance()->processMouse(button,state,x,y);
}
void GeVisualizer::motionFunc(int x, int y) {
    GeVisualizer::instance()->processMotion(x,y);
}

GeVisualizer::GeVisualizer() {
    _input_mode = IM_NONE;
    _left = -1;
    _right = 1;
    _top = 1;
    _bottom = -1;
    _zoomFactor = 0.9;

    bgcR = 0.8;
    bgcG = 0.8;
    bgcB = 0.8;
    objcR = 0;
    objcG = 0;
    objcB = 1;
    objcRsel = 0.07;
    objcGsel = 0.6;
    objcBsel = 0.95;
    objcRfix = 0;
    objcGfix = 0;
    objcBfix = 0;
    objcRfixsel = 0.5;
    objcGfixsel = 0.5;
    objcBfixsel = 0.5;
    _showGrid = false;
}
void GeVisualizer::execute() {
    glutMainLoop();
}
void GeVisualizer::init(int argc, char* argv[]) {
    glutInit(&argc, argv);

    glutInitWindowPosition(0,0);
    //glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(800,800);
    glutCreateWindow("Geos");

    glutDisplayFunc(displayFunc);
    glutKeyboardFunc(keyboardFunc);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(motionFunc);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void GeVisualizer::setController(georis::Controller *controller) {
    if (controller != nullptr)
        _controller = controller;
}
void GeVisualizer::show() {
    setZoom();
    clear();
    showGrid();
    showMode();
    if (_controller) _controller->updateView();

    if (glGetError() == GL_NO_ERROR) {
        glutSwapBuffers();
        glFlush();
    }
}
void GeVisualizer::processMouse(int button, int state, int x, int y) {
    double px, py;
    win2int(x,y,px,py);
    switch (button) {
    case GLUT_LEFT_BUTTON:
        switch (state) {
        case GLUT_DOWN:
				_curx = _begx = px;
				_cury = _begy = py;
            switch(_input_mode) {
            case IM_LINE:
            case IM_CIRCLE:
                break;
            case IM_NONE:
                if (_controller) {
                    double precision = 5*minPixelRes();
                    MOOLOG << "GeVisualizer::processMouse precision = " << precision << std::endl;
                    UID uid;
                    /*
                    if (_controller->selectObj(px,py,precision,&uid) ) {
								_selected = true;
                        _lastSelUid = uid;
                        MOOLOG << "GeVisualizer::processMouse selected = " << uid << std::endl;
                    }
                    */
                }

            }
            break;
        case GLUT_UP:
            switch (_input_mode) {
            case IM_POINT:
                if (_controller) {
                    //UID uid = _controller->addPoint(px,py);
                    //MOOLOG << "GeVisualizer::processMouse added point, uid = " << uid << std::endl;
                }
                _input_mode = IM_NONE;
                break;
            case IM_LINE:
                if ( sqrt((_begx-px)*(_begx-px) + (_begy-py)*(_begy-py)) > 2*minPixelRes())
                    if (_controller) {
                        //UID uid = _controller->addLine(_begx,_begy,px,py);
                        //MOOLOG << "GeVisualizer::processMouse added line, uid = " << uid << std::endl;
                    }
                _input_mode = IM_NONE;
                _begx = _curx;
                _begy = _cury;
                break;
            case IM_CIRCLE:
                double r = sqrt((_begx-px)*(_begx-px) + (_begy-py)*(_begy-py));
                if ( r > 2*minPixelRes())
                    if (_controller) {
                        //UID uid = _controller->addCircle(_begx,_begy,r);
                        //MOOLOG << "GeVisualizer::processMouse added circle, uid = " << uid << std::endl;
                    }
                _input_mode = IM_NONE;
                _begx = _curx;
                _begy = _cury;
                break;
            }
        }
        break;
//	case GLUT_MIDDLE_BUTTON:
    case GLUT_RIGHT_BUTTON:
        //printf("Right button ");
        break;
    case 3: // scroll forward
        if (state == GLUT_UP)
            zoomOut(px,py);
        break;
    case 4: // scroll backward
        if (state == GLUT_UP)
            zoomIn(px,py);
        break;

    default:
        ;
        //printf("Button code %d ",button);
    }
    glutPostRedisplay();
    //   printf("at x: %d, y: %d, button: %d, state: %d\n",x,y,button,state);
    //   printf("t: %f, b: %f, l: %f, r %f\n",_top, _bottom,_left,_right);
}
void GeVisualizer::processKeyboard(GLubyte key,GLint x,GLint y) {
    switch (key) {
    case 27: // ESC
        if ( _input_mode != IM_NONE ) {
            _input_mode = IM_NONE;
            glutPostRedisplay();
        }
        break;
    case 127: // DEL
        if (_controller)
            _controller->deleteSelected();
        break;
    case 'q':
    case 'Q':
        exit(key);
        break;
    case 'l':
    case 'L':
        if ( _input_mode == IM_NONE )
            _input_mode = IM_LINE;
        break;
    case 'p':
    case 'P':
        if ( _input_mode == IM_NONE )
            _input_mode = IM_POINT;
        break;
    case 'c':
    case 'C':
        if ( _input_mode == IM_NONE )
            _input_mode = IM_CIRCLE;
        break;
    case 'f':
        if ( _input_mode == IM_NONE )
            //if (_controller) _controller->addConstraint(CT_FIX)
                    ;
        break;
    case 'z':
        if ( _input_mode == IM_NONE )
            //if (_controller) _controller->addConstraint(CT_PARALLEL)
            ;
        break;
    case 'd':
        if ( _input_mode == IM_NONE )
            //if (_controller) _controller->addConstraint(CT_DISTANCE,1)
            ;
        break;
    case 'o':
        if ( _input_mode == IM_NONE )
            //if (_controller) _controller->addConstraint(CT_ORTHO)
            ;
        break;
    case 'v':
        if ( _input_mode == IM_NONE )
            //if (_controller) _controller->addConstraint(CT_VERTICAL)
                    ;
        break;
    case 'h':
        if ( _input_mode == IM_NONE )
            //if (_controller) _controller->addConstraint(CT_HORIZONTAL)
            ;
        break;
    case 'x':
        if ( _input_mode == IM_NONE )
            //if (_controller) _controller->addConstraint(CT_DISTANCE,0)
            ;
        break;
    case 't':
        if ( _input_mode == IM_NONE )
            //if (_controller) _controller->addConstraint(CT_TANGENT)
            ;
        break;



    }
//LOG << (int)key << std::endl;
    glutPostRedisplay();
}
void GeVisualizer::processMotion(int x,int y) {
	double px, py;
   win2int(x,y,px,py);
   MOOLOG << "Process motion" << std::endl;
   if ( _selected && _controller ) {
        //_controller->moveObj(_lastSelUid, px - _curx,py - _cury);
   }
   _curx = px;
	_cury = py;
	glutPostRedisplay();
}
void GeVisualizer::clear() {
    glClearColor(bgcR,bgcG ,bgcB ,0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GeVisualizer::showGrid() {
    if ( _showGrid ){
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
}
void GeVisualizer::zoomIn(double px,double py) {
    _left = _zoomFactor*_left + px*(1 -_zoomFactor);
    _right = _zoomFactor*_right + px*(1 - _zoomFactor);
    _bottom  = _zoomFactor*_bottom + py*(1-_zoomFactor);
    _top = _zoomFactor*_top + py*(1-_zoomFactor);
    glutPostRedisplay();
}
void GeVisualizer::zoomOut(double px,double py) {
    _left = 1/_zoomFactor*_left + px*(1 -1/_zoomFactor);
    _right = 1/_zoomFactor*_right + px*(1 - 1/_zoomFactor);
    _bottom  = 1/_zoomFactor*_bottom + py*(1-1/_zoomFactor);
    _top = 1/_zoomFactor*_top + py*(1-1/_zoomFactor);
    glutPostRedisplay();
}

void GeVisualizer::setZoom() {
    glLoadIdentity();
    glOrtho(_left,_right,_bottom,_top,-1.0,1.0);
//LOG << "GeVisualizer::setZoom - left = " << _left << ", top = " << _top << ", right = " << _right << ", bottom = " << _bottom << std::endl;
    glMatrixMode(GL_MODELVIEW);
}
void GeVisualizer::showMode() {
    if (_input_mode != IM_NONE) {
        glColor3f( 1, 0, 0 );
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();

        glOrtho(0,800,0,800,-1, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        //glTranslatef(50,50,0);
        glRasterPos2f(10, 750);
        switch (_input_mode) {
        case IM_POINT:
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,'P');
            break;
        case IM_LINE:
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,'L');
            break;
        case IM_CIRCLE:
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,'C');
        }
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        switch (_input_mode) {
        case IM_LINE:
            drawLine(&_begx,&_begy,&_curx,&_cury);
            break;
        case IM_CIRCLE:
            double r = sqrt((_begx - _curx)*(_begx - _curx) + (_begy - _cury)*(_begy - _cury));
            drawCircle(&_begx,&_begy,&r);
        }


    }

}
void GeVisualizer::win2int(int wx,int wy,double &ix,double &iy) {
    int _wWidth = glutGet(GLUT_WINDOW_WIDTH);
    int _wHeight = glutGet(GLUT_WINDOW_HEIGHT);

    ix = _left + wx*(_right - _left)/_wWidth;
    iy = _top - wy*(_top - _bottom)/_wHeight;
}
void GeVisualizer::drawPoint(const double *px,const double *py,unsigned status) {
    setColor(status & MODE_SELECTED, status & MODE_FIXED);
    glPointSize(5.0f);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glBegin(GL_POINTS);
    glVertex2d(*px,*py);
    glEnd();
    glDisable(GL_POINT_SMOOTH);
}
void GeVisualizer::drawLine(const double *px1,const double *py1,const double *px2,const double *py2,unsigned status) {
    setColor(status & MODE_SELECTED, status & MODE_FIXED);
    glLineWidth(2.1f);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_LINES);
    glVertex2d(*px1,*py1);
    glVertex2d(*px2,*py2);
    glEnd();
    //glDisable(GL_POINT_SMOOTH);
}
void GeVisualizer::drawCircle(const double *px,const double *py,const double *r,unsigned status) {
    setColor(status & MODE_SELECTED, status & MODE_FIXED);
    int num_segments = 100;

    float theta = 2 * 3.1415926 / float(num_segments);
    float tangetial_factor = tanf(theta);//calculate the tangential factor

    float radial_factor = cosf(theta);//calculate the radial factor

    float x = *r;//we start at angle = 0

    float y = 0;

    glLineWidth(2.1);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glBegin(GL_LINE_LOOP);
    for(int ii = 0; ii < num_segments; ii++) 	{
        glVertex2f(x + *px, y + *py);//output vertex

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
void GeVisualizer::setColor(bool selected,bool fixed) {
    if ( !selected ) {
        if ( fixed )
            glColor3f( objcRfix,objcGfix,objcBfix);
        else
            glColor3f( objcR, objcG, objcB );
    } else {
        if ( fixed )
            glColor3f( objcRfixsel,objcGfixsel,objcBfixsel);
        else
            glColor3f( objcRsel, objcGsel, objcBsel );
    }
}

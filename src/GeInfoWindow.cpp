#include "GeInfoWindow.h"
#include "../res/xpms.h"
#include "controller.h"
#include <FL/fl_ask.H>

GeInfoWindow::GeInfoWindow(int x,int y,int w,int h,const char *l):Fl_Window(x,y,w,h,l)
{
    _controller = nullptr;

    m_pBrowserSelected = new Fl_Multi_Browser(3,3,w-10,60);
    m_pBrowserComCon = new Fl_Multi_Browser(3,75,w-10,60);

    m_pBrowserSelected->textsize(10);
    m_pBrowserComCon->textsize(10);

    resizable(*m_pBrowserSelected);
    resizable(*m_pBrowserComCon);


    m_pActionToolbar = new Fl_Toolbar(2,140);
    m_pActionToolbar->size(32,32);
    m_pActionToolbar->type(Fl_Pack::VERTICAL);

    _constrbuttons[georis::ConstraintType::CT_FIX] = m_pActionToolbar->AddImageButton("Зафиксировать",new Fl_Pixmap(constrFix_xpm),new Fl_Pixmap(constrFix_selected_xpm), cbConstrFix);
    _constrbuttons[georis::ConstraintType::CT_DIMENSION] = m_pActionToolbar->AddImageButton("Размер",new Fl_Pixmap(constrDim_xpm),new Fl_Pixmap(constrDim_selected_xpm), cbConstrDim);
    _constrbuttons[georis::ConstraintType::CT_VERTICAL] = m_pActionToolbar->AddImageButton("Вертикально",new Fl_Pixmap(constrVertical_xpm),new Fl_Pixmap(constrVertical_selected_xpm), cbConstrVertical);
    _constrbuttons[georis::ConstraintType::CT_HORIZONTAL] = m_pActionToolbar->AddImageButton("Горизонтально",new Fl_Pixmap(constrHorizontal_xpm),new Fl_Pixmap(constrHorizontal_selected_xpm), cbConstrHorizontal);
    _constrbuttons[georis::ConstraintType::CT_PARALLEL] = m_pActionToolbar->AddImageButton("Параллельно",new Fl_Pixmap(constrParallel_xpm),new Fl_Pixmap(constrParallel_selected_xpm), cbConstrParallel);
    _constrbuttons[georis::ConstraintType::CT_ORTHO] = m_pActionToolbar->AddImageButton("Перпендикулярно",new Fl_Pixmap(constrOrtho_xpm),new Fl_Pixmap(constrOrtho_selected_xpm), cbConstrOrtho);
    _constrbuttons[georis::ConstraintType::CT_COINCIDENT] = m_pActionToolbar->AddImageButton("Принадлежит",new Fl_Pixmap(constrCoincident_xpm),new Fl_Pixmap(constrCoincident_selected_xpm), cbConstrCoincident);
    _constrbuttons[georis::ConstraintType::CT_COLLINEAR] = m_pActionToolbar->AddImageButton("Коллинеарно",new Fl_Pixmap(constrCollinear_xpm),new Fl_Pixmap(constrCollinear_selected_xpm), cbConstrCollinear);
    _constrbuttons[georis::ConstraintType::CT_TANGENT] = m_pActionToolbar->AddImageButton("Касательно",new Fl_Pixmap(constrTangent_xpm),new Fl_Pixmap(constrTangent_selected_xpm), cbConstrTangent);
    _constrbuttons[georis::ConstraintType::CT_EQUAL] = m_pActionToolbar->AddImageButton("Равны",new Fl_Pixmap(constrEqual_xpm),new Fl_Pixmap(constrEqual_selected_xpm), cbConstrEqual);
    _constrbuttons[georis::ConstraintType::CT_ANGLE] = m_pActionToolbar->AddImageButton("Угол",new Fl_Pixmap(constrAngle_xpm),new Fl_Pixmap(constrAngle_selected_xpm), cbConstrAngle);
    _constrbuttons[georis::ConstraintType::CT_SYMMETRIC] = m_pActionToolbar->AddImageButton("Симметрично",new Fl_Pixmap(constrSymmetric_xpm),new Fl_Pixmap(constrSymmetric_selected_xpm), cbConstrSymmetric);

    m_pActionToolbar->hide();
    for (auto bu : _constrbuttons)
        bu.second->hide();

    end();

    //border(10);
    box(FL_BORDER_BOX);

}

void GeInfoWindow::setAvailConstraints(const std::vector<georis::ConstraintType> &constr){

    m_pActionToolbar->show();
    for (auto bu : _constrbuttons)
        bu.second->hide();
    for (auto ct: constr)
        _constrbuttons[ct]->show();

    redraw();
}

void GeInfoWindow::setSelectedObjs(const std::map<UID,std::string> &objNames){
    m_pBrowserSelected->clear();
    for (auto it : objNames)
        m_pBrowserSelected->add(it.second.c_str());
}
void GeInfoWindow::setSelectedConstraints(const std::map<UID, std::string> &constrNames){
    m_pBrowserComCon->clear();
    for (auto it : constrNames)
        m_pBrowserComCon->add(it.second.c_str());
}

void GeInfoWindow::cbConstrVertical(Fl_Widget*w, void*d) {
    if (((GeInfoWindow*)(w->parent()->parent()))->_controller) ((GeInfoWindow*)(w->parent()->parent()))->_controller->constrainSelected(georis::CT_VERTICAL);
    if ((GeInfoWindow*)(w->parent()->parent())) ((GeInfoWindow*)(w->parent()->parent()))->parent()->damage();
}
void GeInfoWindow::cbConstrHorizontal(Fl_Widget*w, void*d) {
    if (((GeInfoWindow*)(w->parent()->parent()))->_controller) ((GeInfoWindow*)(w->parent()->parent()))->_controller->constrainSelected(georis::CT_HORIZONTAL);
    if ((GeInfoWindow*)(w->parent()->parent())) ((GeInfoWindow*)(w->parent()->parent()))->parent()->redraw();
}

void GeInfoWindow::cbConstrParallel(Fl_Widget*w, void*d) {
    if (((GeInfoWindow*)(w->parent()->parent()))->_controller) ((GeInfoWindow*)(w->parent()->parent()))->_controller->constrainSelected(georis::CT_PARALLEL);
    if ((GeInfoWindow*)(w->parent()->parent())) ((GeInfoWindow*)(w->parent()->parent()))->parent()->redraw();
}
void GeInfoWindow::cbConstrOrtho(Fl_Widget*w, void*d) {
    if (((GeInfoWindow*)(w->parent()->parent()))->_controller) ((GeInfoWindow*)(w->parent()->parent()))->_controller->constrainSelected(georis::CT_ORTHO);
    if ((GeInfoWindow*)(w->parent()->parent())) ((GeInfoWindow*)(w->parent()->parent()))->parent()->redraw();
}
void GeInfoWindow::cbConstrCoincident(Fl_Widget*w, void*d) {
    if (((GeInfoWindow*)(w->parent()->parent()))->_controller) ((GeInfoWindow*)(w->parent()->parent()))->_controller->constrainSelected(georis::CT_DISTANCE,0);
    if ((GeInfoWindow*)(w->parent()->parent())) ((GeInfoWindow*)(w->parent()->parent()))->parent()->redraw();
}
void GeInfoWindow::cbConstrCollinear(Fl_Widget*w, void*d) {
    if (((GeInfoWindow*)(w->parent()->parent()))->_controller) ((GeInfoWindow*)(w->parent()->parent()))->_controller->constrainSelected(georis::CT_COLLINEAR);
    if ((GeInfoWindow*)(w->parent()->parent())) ((GeInfoWindow*)(w->parent()->parent()))->parent()->redraw();
}
void GeInfoWindow::cbConstrTangent(Fl_Widget*w, void*d) {
    if (((GeInfoWindow*)(w->parent()->parent()))->_controller) ((GeInfoWindow*)(w->parent()->parent()))->_controller->constrainSelected(georis::CT_TANGENT);
    if ((GeInfoWindow*)(w->parent()->parent())) ((GeInfoWindow*)(w->parent()->parent()))->parent()->redraw();
}
void GeInfoWindow::cbConstrEqual(Fl_Widget*w, void*d) {
    if (((GeInfoWindow*)(w->parent()->parent()))->_controller) ((GeInfoWindow*)(w->parent()->parent()))->_controller->constrainSelected(georis::CT_EQUAL);
    if ((GeInfoWindow*)(w->parent()->parent())) ((GeInfoWindow*)(w->parent()->parent()))->parent()->redraw();
}
void GeInfoWindow::cbConstrSymmetric(Fl_Widget*w, void*d) {
    if (((GeInfoWindow*)(w->parent()->parent()))->_controller) ((GeInfoWindow*)(w->parent()->parent()))->_controller->constrainSelected(georis::CT_SYMMETRIC);
    if ((GeInfoWindow*)(w->parent()->parent())) ((GeInfoWindow*)(w->parent()->parent()))->parent()->redraw();
}
void GeInfoWindow::cbConstrFix(Fl_Widget*w, void*d) {
    if (((GeInfoWindow*)(w->parent()->parent()))->_controller) ((GeInfoWindow*)(w->parent()->parent()))->_controller->constrainSelected(georis::CT_FIX);
    if ((GeInfoWindow*)(w->parent()->parent())) ((GeInfoWindow*)(w->parent()->parent()))->parent()->redraw();
}
void GeInfoWindow::cbConstrDim(Fl_Widget*w, void*d) {
    const char* res = fl_input("Введите размер","1.0");
    if (res != 0){
        double dim = strtod(res,0);
        if (((GeInfoWindow*)(w->parent()->parent()))->_controller) ((GeInfoWindow*)(w->parent()->parent()))->_controller->constrainSelected(georis::CT_DIMENSION,dim);
        if ((GeInfoWindow*)(w->parent()->parent())) ((GeInfoWindow*)(w->parent()->parent()))->parent()->redraw();
    }
}
void GeInfoWindow::cbConstrAngle(Fl_Widget*w, void*d) {
    const char* res = fl_input("Введите угол","1.0");
    if (res != 0){
        double dim = strtod(res,0);
        if (((GeInfoWindow*)(w->parent()->parent()))->_controller) ((GeInfoWindow*)(w->parent()->parent()))->_controller->constrainSelected(georis::CT_ANGLE,dim);
        if ((GeInfoWindow*)(w->parent()->parent())) ((GeInfoWindow*)(w->parent()->parent()))->parent()->redraw();
    }
}



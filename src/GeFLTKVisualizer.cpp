#include "GeFLTKVisualizer.h"

#include <FL/Fl_Group.H>
#include <FL/Fl_Native_File_Chooser.H>

#include "mooLog.h"
#include "georis.h"
#include "../res/xpms.h"

georis::GeFLTKVisualizer::GeFLTKVisualizer(int W, int H, const char*L) : Fl_Double_Window(W, H, L) {
    m_controller = nullptr;
    resizable(this);

    Fl_Group * group = new Fl_Group(20,40,W-20,H-20);

    _infowin = new GeInfoWindow(5,40,100,600);
    _infowin->hide();

    _glWindow = new GeGlWindow(_infowin->w() + 5,40,W - (_infowin->w() + 5),H-40);
    //_glWindow = new GeGlWindow(5 ,40, W - 5,H-40);
    resizable(_glWindow);
    _glWindow->hide();

    group->end();

    _toolbar = new Fl_Toolbar(0,0);

    Fl_Button *bu = _toolbar->AddButton("Новый",cbFileNew);
    bu->label("@+2filenew");
    bu = _toolbar->AddButton("Открыть",cbFileOpen);
    bu->label("@+2fileopen");    
    bu = _toolbar->AddButton("Сохранить",cbFileSave);
    bu->label("@+2filesave");
    bu = _toolbar->AddButton("Отмена",cbUndo);
    bu->label("@+2undo");
    bu = _toolbar->AddButton("Повтор",cbRedo);
    bu->label("@+2redo");



    _toolbar->addDivider();
    _modebuttons[IM_POINT] = _toolbar->AddCheckButton("Точка",new Fl_Pixmap(drawPoint_xpm),new Fl_Pixmap(drawPoint_selected_xpm), cbDrawPoint,1);
    _modebuttons[IM_SEGMENT] = _toolbar->AddCheckButton("Отрезок",new Fl_Pixmap(drawLine_xpm),new Fl_Pixmap(drawLine_selected_xpm), cbDrawLine,1);
    _toolbar->addDivider();
    _modebuttons[IM_CIRCLE] = _toolbar->AddCheckButton("Окружность",new Fl_Pixmap(drawCircle_xpm),new Fl_Pixmap(drawCircle_selected_xpm), cbDrawCircle,1);
    _modebuttons[IM_ARC] = _toolbar->AddCheckButton("Дуга",new Fl_Pixmap(drawArc_xpm),new Fl_Pixmap(drawArc_selected_xpm), cbDrawArc,1);
    _toolbar->addDivider();
    _modebuttons[IM_RECT] = _toolbar->AddCheckButton("Прямоугольник",new Fl_Pixmap(drawRect_xpm),new Fl_Pixmap(drawRect_selected_xpm), cbDrawRect,1);

    _statusbar = new Fl_Box(0,H-20,W,20);
    _statusbar->box(FL_FLAT_BOX); // make the box 'flat' (no decorations)
    _statusbar->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);// align status text to left/top of widget
    _statusbar->color(48-2); // slightly darker than default FLTK widget color (48)

    _glWindow->setStatusBar(_statusbar);

    setStatusMessage("Hello");
    end();
}

georis::GeFLTKVisualizer::~GeFLTKVisualizer() {
    if (_glWindow) delete _glWindow;
}
void georis::GeFLTKVisualizer::setController(georis::Controller *ctrl) {
    if (m_controller == nullptr && ctrl != nullptr) {
        m_controller = ctrl;
        _glWindow->show();
        _glWindow->setController(ctrl);
        _infowin->setController(ctrl);
    }
}
int georis::GeFLTKVisualizer::handle(int event) {
    switch (event) {
    case FL_KEYBOARD:
        return processKeyboard(Fl::event_key());
    }
	return Fl_Double_Window::handle(event);
}

void georis::GeFLTKVisualizer::setInputMode(InputMode mode){
    //MOOLOG << "GeFLTKVisualizer::setInputMode with mode = " << mode << std::endl;
	_glWindow->setMode(mode);

    for (auto it : _modebuttons )
        it.second->value(0);
    if ( mode != IM_NONE )
        _modebuttons[mode]->value(1);
}
void georis::GeFLTKVisualizer::resize(int x, int y, int w, int h){

    _glWindow->resize(_infowin->w()+5,40,w - (_infowin->w() + 5),h-40);
    //_glWindow->resize(5,40,w - 5,h-40);

}
int georis::GeFLTKVisualizer::processKeyboard(int key) {
    //MOOLOG << "GeFLTKVisualizer::processKeyboard "<< key << " pressed" << std::endl;
    if (m_controller) {
        switch (key) {
        case FL_Escape: // ESC
            m_controller->resetHighlight();
            m_controller->resetSelection();

            setInputMode(IM_NONE);
            break;
        case FL_Delete: // DEL
            if (m_controller)
                m_controller->deleteSelected();
            break;
        case 'q':
        case 'Q':
            exit(key);
            break;
        case 'l':
        case 'L':
            setInputMode(IM_SEGMENT);
            break;
        case 'p':
        case 'P':
            setInputMode(IM_POINT);
            break;
        case 'c':
        case 'C':
            setInputMode(IM_CIRCLE);
            break;
        case 'f':
            //if ( _input_mode == IM_NONE )
            m_controller->constrainSelected(georis::CT_FIX);
            break;
        case 'z':
            //if ( _input_mode == IM_NONE )
            m_controller->constrainSelected(georis::CT_PARALLEL);
            break;
        case 'd':
            //if ( _input_mode == IM_NONE )
            m_controller->constrainSelected(georis::CT_DISTANCE,1);
            break;
        case 'o':
            //if ( _input_mode == IM_NONE )
            m_controller->constrainSelected(georis::CT_ORTHO);
            break;
        case 'v':
            //if ( _input_mode == IM_NONE )
            m_controller->constrainSelected(georis::CT_VERTICAL);
            break;
        case 'h':
            //if ( _input_mode == IM_NONE )
            m_controller->constrainSelected(georis::CT_HORIZONTAL);
            break;
        case 'x':
            //if ( _input_mode == IM_NONE )
            m_controller->constrainSelected(georis::CT_COINCIDENT);
            break;
        case 't':
            //if ( _input_mode == IM_NONE )
            m_controller->constrainSelected(georis::CT_TANGENT);
            break;
        }
    }
   _glWindow->redraw();
	return 1;
}
void georis::GeFLTKVisualizer::drawObject(ObjectType type, const std::vector<double> &param,unsigned status){
    switch (type){
    case georis::OT_POINT:        
        _glWindow->drawPoint(param[0],param[1],status);
        break;
    case georis::OT_SEGMENT:
        _glWindow->drawLine(param[0],param[1],param[2],param[3],status);
        break;
    case georis::OT_CIRCLE:
        _glWindow->drawCircle(param[0],param[1],param[2],status);
        break;
    case georis::OT_ARC:
        _glWindow->drawArc(param[0],param[1],param[2],param[3],param[4],param[5],status);
        break;

    default:
        ;
    }

}
void georis::GeFLTKVisualizer::displayConstraint(DCType type, double value,const std::vector<double> &param,unsigned status){

    char buff[32];
    snprintf(buff,sizeof(buff),"%.2f",value);

    switch (type){
    case georis::DCT_LINDIM:{
        _glWindow->drawDimLine(param[0],param[1],param[2],param[3],param[4],param[5],buff,status);
        break;
    }
    case georis::DCT_CIRCDIM:
        _glWindow->drawRadiusDimLine(param[0],param[1],value,param[2],param[3],buff,status);
        break;
    case georis::DCT_ANGLEDIM:
        _glWindow->drawAngleDimLine(param[0],param[1],param[2],param[3],param[4],param[5],buff,status);
        break;

    defaut:
        ;
    }
}

void georis::GeFLTKVisualizer::cbFileNew(Fl_Widget*w, void*d) {
    ((GeFLTKVisualizer*)(w->parent()->parent()))->setInputMode(IM_NONE);
    if (((GeFLTKVisualizer*)(w->parent()->parent()))->m_controller) ((GeFLTKVisualizer*)(w->parent()->parent()))->m_controller->createNew();
}

void georis::GeFLTKVisualizer::cbFileOpen(Fl_Widget*w, void*d) {
    ((GeFLTKVisualizer*)(w->parent()->parent()))->setInputMode(IM_NONE);
    Fl_Native_File_Chooser fnfc;
    fnfc.title("Pick a file");
    fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
    //fnfc.filter("Text\t*.txt\nC Files\t*.{cxx,h,c}");
    fnfc.directory(".");           // default directory to use
// Show native chooser
    switch ( fnfc.show() ) {
        case -1:
            MOOLOG << "ERROR: " <<  fnfc.errmsg() << std::endl;
            break;  // ERROR
        case  1:
            MOOLOG << "CANCEL" << std::endl;
            break;  // CANCEL
        default:
            MOOLOG << "PICKED: " << fnfc.filename() << std::endl;
        break;  // FILE CHOSEN
    }
    if (((GeFLTKVisualizer*)(w->parent()->parent()))->m_controller) ((GeFLTKVisualizer*)(w->parent()->parent()))->m_controller->loadFrom(fnfc.filename());
}

void georis::GeFLTKVisualizer::cbFileSave(Fl_Widget*w, void*d) {
    Fl_Native_File_Chooser fnfc;
    fnfc.title("Pick a file");
    fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
    fnfc.filter("Georis XML\t*.grx\nSQL\t*.{sql}");
//    fnfc.filter("Georis XML\t*.grx\nSQL\t*.{cxx,h,c}");
    fnfc.directory(".");           // default directory to use
// Show native chooser
    switch ( fnfc.show() ) {
        case -1:
            MOOLOG << "ERROR: " <<  fnfc.errmsg() << std::endl;
            break;  // ERROR
        case  1:
            MOOLOG << "CANCEL" << std::endl;
            break;  // CANCEL
        default:
            MOOLOG << "PICKED: " << fnfc.filename() << std::endl;
        break;  // FILE CHOSEN
    }
    if (((GeFLTKVisualizer*)(w->parent()->parent()))->m_controller) ((GeFLTKVisualizer*)(w->parent()->parent()))->m_controller->saveTo(fnfc.filename());
}
void georis::GeFLTKVisualizer::cbUndo(Fl_Widget*w, void*d) {

}
void georis::GeFLTKVisualizer::cbRedo(Fl_Widget*w, void*d) {

}



void georis::GeFLTKVisualizer::cbDrawPoint(Fl_Widget*w, void*d) {
	if (!((Fl_ImageCheckButton*)w)->value() )
        ((GeFLTKVisualizer*)(w->parent()->parent()))->setInputMode(IM_NONE);
	else
        ((GeFLTKVisualizer*)(w->parent()->parent()))->setInputMode(IM_POINT);
}
void georis::GeFLTKVisualizer::cbDrawLine(Fl_Widget*w, void*d) {
	if (!((Fl_ImageCheckButton*)w)->value() )
        ((GeFLTKVisualizer*)(w->parent()->parent()))->setInputMode(IM_NONE);
	else
        ((GeFLTKVisualizer*)(w->parent()->parent()))->setInputMode(IM_SEGMENT);
}
void georis::GeFLTKVisualizer::cbDrawCircle(Fl_Widget*w, void*d) {
	if (!((Fl_ImageCheckButton*)w)->value() )
        ((GeFLTKVisualizer*)(w->parent()->parent()))->setInputMode(IM_NONE);
	else
        ((GeFLTKVisualizer*)(w->parent()->parent()))->setInputMode(IM_CIRCLE);
}
void georis::GeFLTKVisualizer::cbDrawArc(Fl_Widget*w, void*d) {
	if (!((Fl_ImageCheckButton*)w)->value() )
        ((GeFLTKVisualizer*)(w->parent()->parent()))->setInputMode(IM_NONE);
	else
        ((GeFLTKVisualizer*)(w->parent()->parent()))->setInputMode(IM_ARC);
}
void georis::GeFLTKVisualizer::cbDrawRect(Fl_Widget*w, void*d) {
    if (!((Fl_ImageCheckButton*)w)->value() )
        ((GeFLTKVisualizer*)(w->parent()->parent()))->setInputMode(IM_NONE);
    else
        ((GeFLTKVisualizer*)(w->parent()->parent()))->setInputMode(IM_RECT);
}

void georis::GeFLTKVisualizer::setSelectionInfo(const std::vector<std::pair<UID,std::string> > &objsSel,
                         const std::vector< std::pair<UID,std::string> > &constrsSel,
                                                const std::vector<georis::ConstraintType>& constrsAvail){

    _infowin->setSelectionInfo(objsSel,constrsSel,constrsAvail);
}

/*
void georis::GeFLTKVisualizer::setAvailConstraints(const std::vector<georis::ConstraintType> &constr) {
    MOOLOG << "GeFLTKVisualizer::setAvailConstraints called with " << constr.size() << " constraints " << std::endl;

}
void georis::GeFLTKVisualizer::setSelectedObjs(const std::map<UID,std::string> &objNames){

}
void georis::GeFLTKVisualizer::setSelectedConstraints(const std::map<UID, std::string> &constrNames){


}
*/

/*
void GeFLTKVisualizer::setSelectionparamProxys(const std::vector<UIConstrInfo> &constrInfos,const GeoObjInfo *objInfo){
    MOOLOG << "GeFLTKVisualizer::setSelectionparamProxys " << constrInfos.size() << std::endl;
	if (objInfo != nullptr ){
        MOOLOG << "\tobjInfo.type " << objInfo->type << std::endl;
		for ( size_t k = 0;k < objInfo->paramProxy.size();++k )
            MOOLOG << "\tobjInfo.paramProxy[" << k << "] " << objInfo->paramProxy[k] << std::endl;
        MOOLOG << "\tobjInfo.status" << objInfo->status << std::endl;
	}

}
*/
void georis::GeFLTKVisualizer::setStatusMessage(const char *msg) {
    _statusbar->copy_label(msg);
    _statusbar->redraw();
}

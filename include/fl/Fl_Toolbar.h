#ifndef FL_TOOLBAR_H
#define FL_TOOLBAR_H
#include <FL/Fl.H>
#include <FL/Fl_Pack.H>
#include "Fl_ImageButton.h"
#include "Fl_ImageCheckButton.h"
#include <FL/Fl_Pixmap.H>
//#include <FL/Fl_Window.H>
//#include <FL/Fl_Widget.H>
#define TOOLBAR_HEIGHT 32
class Fl_Toolbar : public Fl_Pack {
public:
    // CTOR
    Fl_Toolbar(int X, int Y):Fl_Pack(X, Y, 0, TOOLBAR_HEIGHT) {
        type(Fl_Pack::HORIZONTAL);    // horizontal packing of buttons
        //type(Fl_Pack::VERTICAL);    // horizontal packing of buttons
        box(FL_UP_FRAME);
        spacing(4);            // spacing between buttons
        end();
    }
    Fl_Button* AddButton(const char *name, Fl_Callback *cb=0) {
        begin();
        Fl_Button *b = new Fl_Button(0,0,TOOLBAR_HEIGHT,TOOLBAR_HEIGHT,name);
        b->box(FL_FLAT_BOX);    // buttons won't have 'edges'
        //b->box(FL_BORDER_BOX);
        //b->box(FL_THIN_DOWN_BOX);
        b->type(FL_NORMAL_BUTTON);
        b->clear_visible_focus();
        if ( name ) b->tooltip(name);
        if ( cb   ) b->callback(cb, 0);
        switch( type()){
        case Fl_Pack::HORIZONTAL:
            w(w() + b->w() + 3);
           break;
        case Fl_Pack::VERTICAL:
           h(h() + b->h() + 3);
        }
        end();
        return b;
    }
    // ADD A TOOLBAR BUTTON TO THE PACK
    Fl_Image_Button* AddImageButton(const char *name, Fl_Pixmap *uimg=0, Fl_Pixmap *dimg=0, Fl_Callback *cb=0) {
        begin();
        Fl_Image_Button *b = new Fl_Image_Button(0,0,TOOLBAR_HEIGHT,TOOLBAR_HEIGHT);
        b->box(FL_FLAT_BOX);    // buttons won't have 'edges'
        b->clear_visible_focus();
        if ( name ) b->tooltip(name);
        if ( cb   ) b->callback(cb, 0);
        b->up_image(uimg);
        b->down_image(dimg);
        switch( type()){
        case Fl_Pack::HORIZONTAL:
            w(w() + b->w() + 3);
           break;
            case Fl_Pack::VERTICAL:
           h(h() + b->h() + 3);
        }
        end();
        return b;
    }

    Fl_ImageCheckButton* AddCheckButton(const char *name,Fl_Pixmap *uimg=0, Fl_Pixmap *dimg=0, Fl_Callback *cb=0,  int group = 0) {
        begin();
        Fl_ImageCheckButton *b = new Fl_ImageCheckButton(2,0,TOOLBAR_HEIGHT,TOOLBAR_HEIGHT);
        b->setGroup(group);
        //b->box(FL_BORDER_BOX);    // buttons won't have 'edges'
        b->box(FL_FLAT_BOX);    // buttons won't have 'edges'
        b->clear_visible_focus();
        if ( name ) b->tooltip(name);
        if ( cb   ) b->callback(cb, (int*)group);
        b->up_image(uimg);
        b->down_image(dimg);
        switch( type()){
        case Fl_Pack::HORIZONTAL:
            w(w() + b->w() + 3);
           break;
            case Fl_Pack::VERTICAL:
           h(h() + b->h() + 3);
        }
        end();
        return b;
    }

    void addDivider() {
        begin();
        Fl_Button *b = new Fl_Button(0,0,6,TOOLBAR_HEIGHT);
        b->box(FL_FLAT_BOX);    // buttons won't have 'edges'
        b->clear_visible_focus();
        b->color(FL_GRAY + 2);
        //b->hide();
        w(w()+b->w());
        end();
    }

};
#endif // FL_TOOLBAR_H

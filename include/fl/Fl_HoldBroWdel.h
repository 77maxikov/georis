#ifndef FL_MULBROWDEL_H
#define FL_MULBROWDEL_H
#include <FL/Fl.H>
#include <FL/Fl_Hold_Browser.h>


class Fl_HoldBroWDel:public Fl_Hold_Browser{
public:
    Fl_HoldBroWDel(int X,int Y,int W,int H,const char *L=0):Fl_Hold_Browser(X,Y,W,H,L){

    }
    int handle(int event) {
        switch (event) {
        case FL_KEYBOARD:
            return processKeyboardDown(Fl::event_key());
        }
        return Fl_Hold_Browser::handle(event);
    }
    void setDeleteCb(Fl_Callback * cb){m_cb = cb;}
private:
    Fl_Callback *m_cb;
    int processKeyboardDown(int key){
        if ( key == FL_Delete && value() > 0 ){
            m_cb(this,nullptr);
            return 1;
        }
        else
            return 0;
    }
};
#endif

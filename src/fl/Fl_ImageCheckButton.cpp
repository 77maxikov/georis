#include "Fl_ImageCheckButton.h"
#include <FL/Fl_Image.H>
#include <FL/fl_draw.H>

/*bool Fl_ImageCheckButton::isCheck() {
    return _check;
}

void Fl_ImageCheckButton::setCheck(bool value) {
    _check = value;
    redraw();
}
*/

void Fl_ImageCheckButton::draw() {
    // begin - Fl_Button::draw() (box section)
    if (type() == FL_HIDDEN_BUTTON) return;
    Fl_Color col = value() ? selection_color() : color();

    draw_box(value() ? (down_box()?down_box():fl_down(box())) : box(), col);
    // end - Fl_Button::draw() (box section)

    // begin - modification to render up/down image
    Fl_Boxtype box_;
    Fl_Image* pic_ = NULL;


    if(value()) box_ = box();
    else  box_ = box();

    pic_ = value()? down_image():up_image();

    if(pic_) {
        int xx = x() + Fl::box_dx(box_);
        int yy = y() + Fl::box_dy(box_);
        int ww = w() - Fl::box_dw(box_);
        int hh = h() - Fl::box_dh(box_);
        int xpos = (ww - pic_->w()) / 2;
        int ypos = (hh - pic_->h()) / 2;

        fl_push_clip(xx, yy, ww, hh);
        pic_->draw(xx + xpos, yy + ypos);
        fl_pop_clip();
    }

    if (labeltype() == FL_NORMAL_LABEL && value()) {
        Fl_Color c = labelcolor();
        labelcolor(fl_contrast(c, col));
        draw_label();
        labelcolor(c);
    } else draw_label();
    if (Fl::focus() == this) draw_focus();
}


void Fl_ImageCheckButton::updateGroup() {
    if (_group == 0) return;

    int count = parent()->children();

    for (int i = 0; i < count; i++) {
        if (isMyClass(parent()->child(i))) {
            Fl_ImageCheckButton *var = ((Fl_ImageCheckButton*)parent()->child(i));
            var->clear();
        }
    }

}

bool Fl_ImageCheckButton::isMyClass(Fl_Widget *w) {
    if (w == this) return false;

    int *d = (int*)(w->user_data());
    if (d == 0) return false;

    //if (int(d) == _group) return true;
    return false;

    return true;
}

void Fl_ImageCheckButton::setGroup(int value) {
    _group = value;
}

int Fl_ImageCheckButton::getGroup() {
    return _group;
}

int Fl_ImageCheckButton::handle(int event) {
    if (event == FL_PUSH) {
        value(!value());
        redraw();
        if (value()) updateGroup();
    }

    return Fl_Image_Button::handle(event);
}

#include "Fl_ImageButton.h"
#include <FL/Fl_Image.H>
#include <FL/fl_draw.H>
#ifndef NULL
#define NULL 0
#endif
Fl_Image_Button::~Fl_Image_Button(){if (inactimage) delete inactimage;}
void Fl_Image_Button::up_image(Fl_Image* p_upimage)
{
	upimage=p_upimage;
	inactimage = p_upimage->copy();
	inactimage->inactive();
}

void Fl_Image_Button::down_image(Fl_Image* p_downimage)
{
	downimage=p_downimage;
}

Fl_Image* Fl_Image_Button::up_image()
{
	return upimage;
}

Fl_Image* Fl_Image_Button::down_image()
{
	return downimage;
}

Fl_Image_Button::Fl_Image_Button(int a,int b,int c,int d,const char* e):Fl_Button(a,b,c,d,e)
{
	upimage=NULL;
	downimage=NULL;
	inactimage = 0;
	Fl_Button::clear_visible_focus();
}
Fl_Image_Button::Fl_Image_Button(int a,int b,int c,int d):Fl_Button(a,b,c,d)
{
	upimage=NULL;
	downimage=NULL;
	inactimage = 0;
	Fl_Button::clear_visible_focus();
}

/*void Fl_Image_Button::draw() {
	if (value()==0)
	{
		if (upimage!=NULL) {
			this->image(upimage);
		}
	} else {
		if (this->value()==1)
		{
			if (downimage!=NULL) {
				this->image(downimage);
			}
		}
	}
	Fl_Button::draw();
}

*/


void Fl_Image_Button::draw()
{
  // begin - Fl_Button::draw() (box section)
  if (type() == FL_HIDDEN_BUTTON) return;
  Fl_Color col = value() ? selection_color() : color();


  draw_box(value() ? (down_box()?down_box():fl_down(box())) : box(), col);
  // end - Fl_Button::draw() (box section)

  // begin - modification to render up/down image
  Fl_Boxtype box_;
  Fl_Image* pic_;

  if(value())
  {
    box_ = (down_box())? down_box() : fl_down(box());
    pic_ = downimage;
  }
  else
  {
    box_ = box();
    if ( active() )
		pic_ = upimage;
	else
		pic_ = inactimage;
  }

  if(pic_)
  {
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
  // end- modification to render up/down image

  // begin - Fl_Button::draw() (label section)
  if (labeltype() == FL_NORMAL_LABEL && value()) {
    Fl_Color c = labelcolor();
    labelcolor(fl_contrast(c, col));
    draw_label();
    labelcolor(c);
  } else draw_label();
  if (Fl::focus() == this) draw_focus();
  // end - Fl_Button::draw() (label section)
}

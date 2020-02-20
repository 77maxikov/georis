#pragma once

#include "Fl_ImageButton.h"

class Fl_ImageCheckButton: public Fl_Image_Button
{
private:
	//bool _check;
	int _group;
	bool _iredraw;
	void updateGroup();
	bool isMyClass(Fl_Widget *w);
	int handle(int);
public:
	Fl_ImageCheckButton(int a,int b,int c,int d):Fl_Image_Button(a, b, c, d)
	{
		_group = 0;
		_iredraw = false;
	}
	int value(int v){int vv = Fl_Button::value(v);redraw();return vv;}
	char value()const {return Fl_Button::value();}

	void setCheck(bool value);
	void setGroup(int value);
	int getGroup();
	void draw();
};


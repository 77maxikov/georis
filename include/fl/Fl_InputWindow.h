#pragma once
#include <FL/Fl_Window.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>

class GeFLTKInputWindow:public  Fl_Window
{
	 Fl_Input *_input;
	 Fl_Button *_btnOk,*_btnCancel;
public:
	GeFLTKInputWindow(const char *prompt,double data);
	~GeFLTKInputWindow(void);

};


#include "Fl_InputWindow.h"


GeFLTKInputWindow::GeFLTKInputWindow(const char *prompt,double data):Fl_Window(20,20,200,50,prompt){

	begin();
	_input = new  Fl_Input(10,10,60,20);
	_btnOk = new Fl_Button(80,10,40,20,"OK");
	_btnCancel = new Fl_Button(130,10,50,20,"Cancel");
	end();
	set_modal();
}


GeFLTKInputWindow::~GeFLTKInputWindow(void)
{
	delete _input;
}

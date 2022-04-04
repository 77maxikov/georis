#include <iostream>
#include "controller.h"

#include "GeFLTKVisualizer.h"

#define FLTKVIS

int main(int argc,char *argv[]) {
    try{
        georis::Controller controller;
/*
#ifndef FLTKVIS
        GeVisualizer *vis = GeVisualizer::instance();
        vis->init(argc,argv);

        controller.setUI( dynamic_cast<IVisualizer *>(vis) );

        vis->setController(&controller);
        vis->execute();
#else
*/
        Fl::visual(FL_DOUBLE|FL_INDEX);
        int x,y,w,h;
        Fl::screen_xywh(x,y,w,h);
        georis::GeFLTKVisualizer vis(w,h);
        controller.setUI(&vis);
        vis.setController(&controller);        
        vis.show();
        if ( argc > 1 )
            controller.loadFrom(argv[1]);

        Fl::run();
//#endif

    }
    catch  (std::invalid_argument &e){
        MOOLOG << e.what() << std::endl;
    }
    catch  (std::runtime_error &e){
        MOOLOG << e.what() << std::endl;
    }
    catch(...){
        MOOLOG << "Unexpected error!" << std::endl;
    }
    return 0;
}

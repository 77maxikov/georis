cmake_minimum_required(VERSION 3.20)

project(geos)

find_package(FLTK REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)
find_package (Eigen3 3.3 REQUIRED NO_MODULE)


set (GEORIS_SRC
     src/test.cpp
     externals/tinyxml2/tinyxml2.cpp
     src/GeFLTKVisualizer.cpp
     src/GeGlWindow.cpp
     src/GeInfoWindow.cpp
     src/GeosFuncN.cpp
     src/constraints/ConstrEqual.cpp
     src/constraints/ConstrL2L.cpp
     src/constraints/ConstrL2LAngle.cpp
     src/constraints/ConstrL2LEqual.cpp
     src/constraints/ConstrL2LOrtho.cpp
     src/constraints/ConstrL2LOrtho2.cpp
     src/constraints/ConstrL2LParal.cpp
     src/constraints/ConstrP2LDist.cpp
     src/constraints/ConstrP2PDist.cpp
     src/constraints/ConstrPP2LSym.cpp
     src/constraints/Constraint.cpp
     src/controller.cpp
     src/core.cpp
     src/fl/Fl_ImageButton.cpp
     src/fl/Fl_ImageCheckButton.cpp
     src/fl/Fl_InputWindow.cpp
     src/fl/Fl_Toolbar.cpp
     src/fontdata.cpp
     src/iosvgx.cpp
     src/iordb.cpp
     src/mooLog.cpp
     src/objutils.cpp
     src/optim/SolverLM.cpp
     src/optim/SolverNG.cpp
     src/optim/SolverNReg.cpp
     src/uid.cpp
)

add_executable(georis ${GEORIS_SRC})
set_target_properties(georis PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

include_directories(include 
		    include/constraints 
                    include/optim 
                    include/fl 
                    externals/tinyxml2 ${FLTK_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS}   ${FREETYPE_INCLUDE_DIRS} )
target_link_libraries(georis ${FLTK_LIBRARIES} ${OPENGL_LIBRARIES} ${FREETYPE_LIBRARIES} Eigen3::Eigen)

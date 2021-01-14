#-------------------------------------------------
#
# Project created by QtCreator 2014-02-27T19:00:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = georis
TEMPLATE = app
INCLUDEPATH += ../include \
	../include/constraints \
	../include/optim \
    ../include/fl \
    ../externals/tinyxml2 \
	/usr/include/eigen3

SOURCES += ../src/test.cpp \
    ../externals/tinyxml2/tinyxml2.cpp \
   ../src/GeFLTKVisualizer.cpp \
   ../src/GeGlWindow.cpp \
   ../src/GeInfoWindow.cpp \
   ../src/GeosFuncN.cpp \
   ../src/constraints/ConstrEqual.cpp \
   ../src/constraints/ConstrEqualAbs.cpp \
   ../src/constraints/ConstrL2L.cpp \
   ../src/constraints/ConstrL2LAngle.cpp \
   ../src/constraints/ConstrL2LEqual.cpp \
   ../src/constraints/ConstrL2LOrtho.cpp \
   ../src/constraints/ConstrL2LOrtho2.cpp \
   ../src/constraints/ConstrL2LOrthoAbs.cpp \
   ../src/constraints/ConstrL2LParal.cpp \
   ../src/constraints/ConstrP2LDist.cpp \
   ../src/constraints/ConstrP2LDist2.cpp \
   ../src/constraints/ConstrP2LDistAbs.cpp \
   ../src/constraints/ConstrP2PDist.cpp \
   ../src/constraints/ConstrP2PDistAbs.cpp \
   ../src/constraints/Constraint.cpp \
   ../src/controller.cpp \
   ../src/core.cpp \
   ../src/fl/Fl_ImageButton.cpp \
   ../src/fl/Fl_ImageCheckButton.cpp \
   ../src/fl/Fl_InputWindow.cpp \
   ../src/fl/Fl_Toolbar.cpp \
   ../src/iosvgx.cpp \
   ../src/iordb.cpp \
    ../src/mooLog.cpp \
   ../src/objutils.cpp \
   ../src/optim/SolverLM.cpp \
   ../src/optim/SolverNG.cpp \
   ../src/uid.cpp

HEADERS  += \
    ../include/GeGlWindow.h \
    ../include/IFileIO.h \
    ../include/constraints/ConstrEqual.h \
    ../include/constraints/ConstrEqualAbs.h \
    ../include/constraints/ConstrL2L.h \
    ../include/constraints/ConstrL2LAngle.h \
    ../include/constraints/ConstrL2LEqual.h \
    ../include/constraints/ConstrL2LOrtho.h \
    ../include/constraints/ConstrL2LOrtho2.h \
    ../include/constraints/ConstrL2LOrthoAbs.h \
    ../include/constraints/ConstrL2LParal.h \
    ../include/constraints/ConstrP2LDist.h \
    ../include/constraints/ConstrP2LDist2.h \
    ../include/constraints/ConstrP2LDistAbs.h \
    ../include/constraints/ConstrP2PDist.h \
    ../include/constraints/ConstrP2PDistAbs.h \
    ../include/constraints/IConstraint.h \
    ../include/controller.h \
    ../include/core.h \
    ../include/georis.h \
    ../include/errors.h \
    ../include/iordb.h \
    ../include/mooLog.h \
    ../include/objects.h \
    ../include/objinfo.h \
    ../include/uid.h

#LIBS += -lglut
LIBS += -lfltk -lfltk_gl -lglut



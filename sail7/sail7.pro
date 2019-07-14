#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------
CONFIG += qt
QT += opengl xml
TEMPLATE = app
TARGET = sail7

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
    src/globals.cpp \
    src/graph/curve.cpp \
    src/graph/graph.cpp \
    src/graph/graphdlg.cpp \
    src/graph/graphwidget.cpp \
    src/graph/qgraph.cpp \
    src/main.cpp \
    src/mainframe.cpp \
    src/misc/abouts7.cpp \
    src/misc/colorbutton.cpp \
    src/misc/displaysettingsdlg.cpp \
    src/misc/floatedit.cpp \
    src/misc/floateditdelegate.cpp \
    src/misc/gllightdlg.cpp \
    src/misc/linebutton.cpp \
    src/misc/linecbbox.cpp \
    src/misc/linedelegate.cpp \
    src/misc/linepickerdlg.cpp \
    src/misc/moddlg.cpp \
    src/misc/newnamedlg.cpp \
    src/misc/objectpropsdlg.cpp \
    src/misc/polarfilterdlg.cpp \
    src/misc/progressdlg.cpp \
    src/misc/renamedlg.cpp \
    src/misc/selectobjectdlg.cpp \
    src/misc/translatordlg.cpp \
    src/misc/unitsdlg.cpp \
    src/misc/w3dprefsdlg.cpp \
    src/objects/arcball.cpp \
    src/objects/arcspline.cpp \
    src/objects/bezierspline.cpp \
    src/objects/boat.cpp \
    src/objects/boatopp.cpp \
    src/objects/boatpolar.cpp \
    src/objects/body.cpp \
    src/objects/bspline.cpp \
    src/objects/cubicspline.cpp \
    src/objects/frame.cpp \
    src/objects/naca4spline.cpp \
    src/objects/nurbssail.cpp \
    src/objects/nurbssurface.cpp \
    src/objects/panel.cpp \
    src/objects/pointspline.cpp \
    src/objects/quaternion.cpp \
    src/objects/sail.cpp \
    src/objects/sailcutsail.cpp \
    src/objects/sailcutspline.cpp \
    src/objects/sailsection.cpp \
    src/objects/spline.cpp \
    src/objects/vector3d.cpp \
    src/sail7/boatanalysisdlg.cpp \
    src/sail7/boatdlg.cpp \
    src/sail7/boatpolardlg.cpp \
    src/sail7/bodygriddlg.cpp \
    src/sail7/bodyscaledlg.cpp \
    src/sail7/bodytabledelegate.cpp \
    src/sail7/bodytransdlg.cpp \
    src/sail7/gl3dbodydlg.cpp \
    src/sail7/gl3dscales.cpp \
    src/sail7/glcreatebodylists.cpp \
    src/sail7/sail7.cpp \
    src/sail7/saildlg.cpp \
    src/sail7/saildomdoc.cpp \
    src/sail7/sailtabledelegate.cpp \
    src/sail7/sailviewwidget.cpp \
    src/sail7/scalesaildlg.cpp \
    src/sail7/sectionviewwidget.cpp \
    src/sail7application.cpp \
    src/threedwidget.cpp \
    src/twodwidget.cpp \

HEADERS  += \
    src/globals.h \
    src/graph/curve.h \
    src/graph/graph.h \
    src/graph/graphdlg.h \
    src/graph/graphwidget.h \
    src/graph/qgraph.h \
    src/mainframe.h \
    src/misc/abouts7.h \
    src/misc/colorbutton.h \
    src/misc/displaysettingsdlg.h \
    src/misc/floatedit.h \
    src/misc/floateditdelegate.h \
    src/misc/gllightdlg.h \
    src/misc/linebutton.h \
    src/misc/linecbbox.h \
    src/misc/linedelegate.h \
    src/misc/linepickerdlg.h \
    src/misc/moddlg.h \
    src/misc/newnamedlg.h \
    src/misc/objectpropsdlg.h \
    src/misc/polarfilterdlg.h \
    src/misc/progressdlg.h \
    src/misc/renamedlg.h \
    src/misc/selectobjectdlg.h \
    src/misc/translatordlg.h \
    src/misc/unitsdlg.h \
    src/misc/w3dprefsdlg.h \
    src/objects/arcball.h \
    src/objects/arcspline.h \
    src/objects/bezierspline.h \
    src/objects/boat.h \
    src/objects/boatopp.h \
    src/objects/boatpolar.h \
    src/objects/body.h \
    src/objects/bspline.h \
    src/objects/cubicspline.h \
    src/objects/frame.h \
    src/objects/naca4spline.h \
    src/objects/nurbssail.h \
    src/objects/nurbssurface.h \
    src/objects/panel.h \
    src/objects/pointspline.h \
    src/objects/quaternion.h \
    src/objects/rectangle.h \
    src/objects/sail.h \
    src/objects/sailcutsail.h \
    src/objects/sailcutspline.h \
    src/objects/sailsection.h \
    src/objects/spline.h \
    src/objects/vector3d.h \
    src/params.h \
    src/sail7/boatanalysisDlg.h \
    src/sail7/boatdlg.h \
    src/sail7/boatpolardlg.h \
    src/sail7/bodygriddlg.h \
    src/sail7/bodyscaledlg.h \
    src/sail7/bodytabledelegate.h \
    src/sail7/bodytransdlg.h \
    src/sail7/gl3dbodydlg.h \
    src/sail7/gl3dscales.h \
    src/sail7/glcreatebodylists.h \
    src/sail7/sail7.h \
    src/sail7/saildlg.h \
    src/sail7/saildomdoc.h \
    src/sail7/sailtabledelegate.h \
    src/sail7/sailwwidget.h \
    src/sail7/scalesaildlg.h \
    src/sail7/sectionviewwidget.h \
    src/sail7application.h \
    src/threedwidget.h \
    src/twodwidget.h \

RESOURCES +=  sail7.qrc


OBJECTS_DIR = ./objects
MOC_DIR     = ./moc
RCC_DIR     = ./rcc
DESTDIR     = .

win32 {
    RC_FILE = win/Sail7.rc
}


linux-g++{
# VARIABLES
    isEmpty(PREFIX):PREFIX = /usr/local
    BINDIR = $$PREFIX/bin
    DATADIR = $$PREFIX/share

    # MAKE INSTALL
    INSTALLS += target
    target.path = $$BINDIR
}




#QMAKE_CFLAGS+=-pg
#QMAKE_CXXFLAGS+=-pg
#QMAKE_LFLAGS+=-pg

OTHER_FILES += \
        doc/ReleaseNotes.txt


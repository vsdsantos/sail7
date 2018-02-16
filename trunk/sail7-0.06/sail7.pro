#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------
CONFIG += qt
QT += opengl xml
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
    src/twodwidget.cpp \
	src/threedwidget.cpp \
	src/sail7Application.cpp \
    src/mainframe.cpp \
    src/main.cpp \
    src/globals.cpp \
    src/graph/qgraph.cpp \
    src/graph/graphwidget.cpp \
    src/graph/graphdlg.cpp \
    src/graph/graph.cpp \
    src/graph/curve.cpp \
	src/misc/AboutS7.cpp \
	src/misc/W3dPrefsDlg.cpp \
    src/misc/UnitsDlg.cpp \
    src/misc/TranslatorDlg.cpp \
    src/misc/RenameDlg.cpp \
    src/misc/ProgressDlg.cpp \
    src/misc/PolarFilterDlg.cpp \
    src/misc/ObjectPropsDlg.cpp \
    src/misc/NewNameDlg.cpp \
    src/misc/ModDlg.cpp \
    src/misc/LinePickerDlg.cpp \
    src/misc/LineDelegate.cpp \
    src/misc/FloatEditDelegate.cpp \
    src/misc/FloatEdit.cpp \
    src/misc/LineCbBox.cpp \
    src/misc/LineButton.cpp \
    src/misc/GLLightDlg.cpp \
    src/misc/DisplaySettingsDlg.cpp \
    src/misc/ColorButton.cpp \
    src/objects/Quaternion.cpp \
	src/objects/CVector.cpp \
    src/objects/Panel.cpp \
    src/objects/ArcBall.cpp \
	src/objects/Sail.cpp \
	src/objects/Boat.cpp \
	src/objects/BoatPolar.cpp \
	src/objects/BoatOpp.cpp \
    src/sail7/sail7.cpp \
    src/sail7/BoatPolarDlg.cpp \
    src/sail7/BoatAnalysisDlg.cpp \
    src/sail7/GL3DScales.cpp \
    src/sail7/BodyTransDlg.cpp \
    src/sail7/BodyTableDelegate.cpp \
    src/sail7/BodyScaleDlg.cpp \
    src/sail7/BodyGridDlg.cpp \
    src/objects/CubicSpline.cpp \
    src/objects/BSpline.cpp \
    src/sail7/SailDlg.cpp \
    src/objects/SailSection.cpp \
    src/sail7/BoatDlg.cpp \
    src/objects/Spline.cpp \
    src/objects/SailcutSpline.cpp \
    src/objects/PointSpline.cpp \
    src/sail7/SailTableDelegate.cpp \
    src/objects/BezierSpline.cpp \
    src/objects/Naca4Spline.cpp \
    src/objects/ArcSpline.cpp \
	src/sail7/ScaleSailDlg.cpp \
    src/misc/SelectObjectDlg.cpp \
    src/objects/SailcutSail.cpp \
    src/objects/NURBSSail.cpp \
    src/sail7/GLCreateBodyLists.cpp \
    src/sail7/GL3dBodyDlg.cpp \
	src/objects/NURBSSurface.cpp \
	src/objects/Frame.cpp \
	src/objects/Body.cpp \
    src/sail7/SailViewWidget.cpp \
    src/sail7/SectionViewWidget.cpp \
    src/sail7/SailDomDoc.cpp

HEADERS  += \
    src/twodwidget.h \
	src/threedwidget.h \
	src/sail7Application.h \
    src/mainframe.h \
    src/params.h \
    src/globals.h \
    src/graph/graphwidget.h \
    src/graph/graphdlg.h \
    src/graph/graph.h \
    src/graph/curve.h \
    src/graph/qgraph.h \
    src/misc/W3dPrefsDlg.h \
    src/misc/UnitsDlg.h \
    src/misc/TranslatorDlg.h \
    src/misc/RenameDlg.h \
    src/misc/ProgressDlg.h \
    src/misc/PolarFilterDlg.h \
    src/misc/ObjectPropsDlg.h \
    src/misc/NewNameDlg.h \
	src/misc/SelectObjectDlg.h \
	src/misc/ModDlg.h \
    src/misc/LinePickerDlg.h \
    src/misc/LineDelegate.h \
    src/misc/FloatEditDelegate.h \
    src/misc/LineCbBox.h \
    src/misc/LineButton.h \
    src/misc/GLLightDlg.h \
    src/misc/DisplaySettingsDlg.h \
    src/misc/ColorButton.h \
    src/misc/FloatEdit.h \
	src/misc/AboutS7.h \
    src/objects/Quaternion.h \
    src/objects/CRectangle.h \
    src/objects/Panel.h \
    src/objects/CVector.h \
    src/objects/ArcBall.h \
	src/objects/BoatPolar.h \
	src/objects/BoatOpp.h \
	src/objects/Sail.h \
	src/objects/Boat.h \
	src/objects/CubicSpline.h \
	src/objects/BSpline.h \
	src/objects/NURBSSurface.h \
	src/objects/Frame.h \
	src/objects/Body.h \
	src/objects/Spline.h \
	src/objects/SailcutSpline.h \
	src/objects/PointSpline.h \
	src/objects/SailcutSail.h \
	src/objects/NURBSSail.h \
	src/objects/SailSection.h \
	src/objects/BezierSpline.h \
	src/objects/Naca4Spline.h \
	src/objects/ArcSpline.h \
	src/sail7/sail7.h \
	src/sail7/BoatPolarDlg.h \
    src/sail7/BoatAnalysisDlg.h \
	src/sail7/GL3DScales.h \
    src/sail7/BodyTableDelegate.h \
    src/sail7/BodyScaleDlg.h \
    src/sail7/BodyGridDlg.h \
    src/sail7/BodyTransDlg.h \
	src/sail7/SailDlg.h \
    src/sail7/BoatDlg.h \
    src/sail7/SailTableDelegate.h \
	src/sail7/ScaleSailDlg.h \
    src/sail7/GLCreateBodyLists.h \
    src/sail7/GL3dBodyDlg.h \
    src/sail7/SailViewWidget.h \
    src/sail7/SectionViewWidget.h \
    src/sail7/SailDomDoc.h

RESOURCES +=  sail7.qrc

win32 {
	TARGET = Sail7
	RC_FILE = win/Sail7.rc
}

unix { 
    TARGET = sail7
    
    # VARIABLES
    isEmpty(PREFIX):PREFIX = /usr
    BINDIR = $$PREFIX/bin
    DATADIR = $$PREFIX/share
    
    # MAKE INSTALL
    INSTALLS += target
#    target.path = $$BINDIR
}


#QMAKE_CFLAGS+=-pg
#QMAKE_CXXFLAGS+=-pg
#QMAKE_LFLAGS+=-pg

OTHER_FILES += \
    doc/ReleaseNotes.txt


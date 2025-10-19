#-------------------------------------------------
#
# Project created by QtCreator 2025-06-02T22:24:56
#
#-------------------------------------------------

QT       += core gui widgets xml network concurrent qml quick quickwidgets quickcontrols2 charts multimedia multimediawidgets


OBJECTS_DIR = ../obj/OBJ
MOC_DIR = ../obj/MOC
UI_DIR = ../obj/UI
RCC_DIR = ../obj/RCC

TARGET = ../bin/RadarDisplay
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

# QGIS includes
INCLUDEPATH += /usr/include/qgis

# QGIS libraries
LIBS += -lqgis_core -lqgis_gui -lqgis_analysis

# QGIS dependencies (if needed)
LIBS += -lgeos_c -lproj -lspatialindex

SOURCES += \
        CoordinateConverter.cpp \
        MapDisplay/canalyticswidget.cpp \
        MapDisplay/cchartswidget.cpp \
        MapDisplay/cconfigpanelwidget.cpp \
        MapDisplay/chealthmonitorwidget.cpp \
        MapDisplay/cinterfacespanelwidget.cpp \
        MapDisplay/cpredictivemaintenancewidget.cpp \
        MapDisplay/cppilayer.cpp \
        MapDisplay/crecordingwidget.cpp \
        MapDisplay/creplaywindow.cpp \
        MapDisplay/csearchbeamlayer.cpp \
        MapDisplay/csimulationwidget.cpp \
        MapDisplay/ctracklayer.cpp \
        MapDisplay/ctracktablewidget.cpp \
        MapDisplay/customgradiantfillsymbollayer.cpp \
        cdatawarehouse.cpp \
        cdrone.cpp \
        cudpreceiver.cpp \
        main.cpp \
        cmapmainwindow.cpp \
        cppiwindow.cpp \
        ccontrolswindow.cpp \
        MapDisplay/cmapcanvas.cpp \
        matrix.cpp

HEADERS += \
        CoordinateConverter.h \
        MapDisplay/canalyticswidget.h \
        MapDisplay/cchartswidget.h \
        MapDisplay/cconfigpanelwidget.h \
        MapDisplay/chealthmonitorwidget.h \
        MapDisplay/cinterfacespanelwidget.h \
        MapDisplay/cpredictivemaintenancewidget.h \
        MapDisplay/cppilayer.h \
        MapDisplay/crecordingwidget.h \
        MapDisplay/creplaywindow.h \
        MapDisplay/csearchbeamlayer.h \
        MapDisplay/csimulationwidget.h \
        MapDisplay/ctracklayer.h \
        MapDisplay/ctracktablewidget.h \
        MapDisplay/customgradiantfillsymbollayer.h \
        cdatawarehouse.h \
        cdrone.h \
        cmapmainwindow.h \
        cppiwindow.h \
        ccontrolswindow.h \
        MapDisplay/cmapcanvas.h \
        cudpreceiver.h \
        globalmacros.h \
        globalstructs.h \
        matrix.h

FORMS += \
        cmapmainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


RESOURCES += \
    myRes.qrc

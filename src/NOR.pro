#-------------------------------------------------
#
# Project created by QtCreator 2020-01-15T19:21:24
#
#-------------------------------------------------


QT       += core gui
QT         += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NOR
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

SOURCES += \
        main.cpp \
        model/calculator.cpp \
        model/component.cpp \
        model/connection.cpp \
        model/description.cpp \
        model/filemanager.cpp \
        model/powersupply.cpp \
        model/resistor.cpp \
        view/aboutbox.cpp \
        view/editview.cpp \
        view/mainwindow.cpp \
        model/networkgraphics.cpp \
        view/networkview.cpp \
        model/componentport.cpp \
        model/rowpiece.cpp \
        model/commands.cpp \
        view/powerview.cpp \
        model/gridobject.cpp

HEADERS += \
        defines.h \
        model/calculator.h \
        model/component.h \
        model/connection.h \
        model/description.h \
        model/filemanager.h \
        model/powersupply.h \
        model/resistor.h \
        view/aboutbox.h \
        view/editview.h \
        view/mainwindow.h \
        model/networkgraphics.h \
        view/networkview.h \
        model/componentport.h \
        model/node.h \
        model/rowpiece.h \
        model/commands.h \
        view/powerview.h \
        model/gridobject.h \
        model/graphicsobject.h

FORMS += \
        view/aboutbox.ui \
        view/editview.ui \
        view/mainwindow.ui \
        view/powerview.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    pictures.qrc

RC_ICONS = img/logo.ico

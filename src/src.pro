#-------------------------------------------------
#
# Project created by QtCreator 2019-04-27T08:19:14
#
#-------------------------------------------------

QT       += core gui dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EveryLauncher
TEMPLATE = app

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


dbus.files = $$PWD/com.gitee.wanywhn.EveryLauncher.xml
dbus.header_flags += -l DBusProxy -i $$PWD/dbusproxy.h
dbus.source_flags += -l DBusProxy

dbus_itface.files= $$PWD/com.gitee.wanywhn.EveryLauncher.xml
dbus_itface.header_flags += -c EveryLauncherInterface
dbus_itface.source_flags += -c EveryLauncherInterface

DBUS_ADAPTORS += dbus
DBUS_INTERFACES +=dbus_itface
#DBUS_INTERFACES +=$$PWD/com.gitee.wanywhn.EveryLauncher.xml

dbus_xmls.path = /usr/share/dbus-1/interfaces
dbus_xmls.files = $$dbus.files

LIBS += -lrecoll -L/usr/lib/recoll
INCLUDEPATH+=../../recoll1-code/src/query\
                ../../recoll1-code/src/utils\
                ../../recoll1-code/src/rcldb\
                ../../recoll1-code/src/internfile\
                ../../recoll1-code/src/unac\
                ../../recoll1-code/src/common\
                ../../recoll1-code/src/qtgui\
#                ../../recoll1-code/src/qtgui\confgui

SOURCES += \
        main.cpp \
        widget.cpp \
    systemtray.cpp \
    preferencewindow.cpp \
    configlistwidget.cpp \
    indexsche.cpp \
    reslistwidget.cpp\
        ../../recoll1-code/src/qtgui/confgui/confgui.cpp\
        ../../recoll1-code/src/qtgui/confgui/confguiindex.cpp\
        ../../recoll1-code/src/qtgui/guiutils.cpp\
    searchline.cpp \
    dbusproxy.cpp\
        everylaunchermonitor_interface.cpp

HEADERS += \
        widget.h \
    config.h \
    systemtray.h \
    preferencewindow.h \
    configlistwidget.h \
    indexsche.h \
    reslistwidget.h\
#        ../../recoll1-code/src/query/docseq.h
        ../../recoll1-code/src/qtgui/confgui/confgui.h\
        ../../recoll1-code/src/qtgui/confgui/confguiindex.h\
        ../../recoll1-code/src/qtgui/guiutils.h\
    searchline.h \
    dbusproxy.h\
        everylaunchermonitor_interface.h

FORMS += \
        widget.ui

#-------------------------------------------------
#
# Project created by QtCreator 2019-04-27T08:19:14
#
#-------------------------------------------------
TARGET = everylauncher
TEMPLATE = app

QT       += core gui dbus webenginewidgets x11extras

LIBS += -lrecoll
LIBS += -lX11 -lXext -lXtst -lQt5Pdf -lQt5PdfWidgets
LIBS += -L$$PWD/../lib

QMAKE_RPATHDIR +=/usr/lib/recoll
CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkwidget xcb xcb-util Qt5Xdg
QMAKE_CXXFLAGS += -std=c++11
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
isEmpty(PREFIX): PREFIX = /usr

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


INCLUDEPATH+=../../recoll1-code/src/query\
                ../../recoll1-code/src/utils\
                ../../recoll1-code/src/rcldb\
                ../../recoll1-code/src/internfile\
                ../../recoll1-code/src/unac\
                ../../recoll1-code/src/common\
                ../../recoll1-code/src/qtgui\
                ../../qtpdf/include\
                ../../qtpdf/include/QtPdf

SOURCES += \
        main.cpp \
        widget.cpp \
    systemtray.cpp \
    preferencewindow.cpp \
    configlistwidget.cpp \
    indexsche.cpp \
    reslistwidget.cpp\
    searchline.cpp \
    dbusproxy.cpp\
        everylaunchermonitor_interface.cpp \
    msortfilterproxymodel.cpp \
    detailedwidget.cpp \
    recollmodel.cpp \
    Detailed/detailedtext.cpp \
    Detailed/preview_w.cpp \
    Detailed/preview_load.cpp \
    Detailed/preview_plaintorich.cpp \
    Detailed/previewtextedit.cpp\
        confgui/confgui.cpp\
        confgui/confguiindex.cpp\
        guiutils.cpp \
    Detailed/desktoppreview.cpp\
    keymonitor.cpp \
    Detailed/pdfpreview.cpp \
    Detailed/imagepreview.cpp

HEADERS += \
        widget.h \
    config.h \
    systemtray.h \
    preferencewindow.h \
    configlistwidget.h \
    indexsche.h \
    reslistwidget.h\
    searchline.h \
    dbusproxy.h\
        everylaunchermonitor_interface.h \
    msortfilterproxymodel.h \
    detailedwidget.h \
    recollmodel.h \
    Detailed/detailedtext.h \
    Detailed/preview_w.h \
    Detailed/preview_load.h \
    Detailed/preview_plaintorich.h \
    Detailed/previewtextedit.h\
        confgui/confgui.h\
        confgui/confguiindex.h\
        guiutils.h \
    Detailed/desktoppreview.h\
    keymonitor.h \
    Detailed/pdfpreview.h \
    Detailed/imagepreview.h


FORMS += \
        widget.ui

dbus.files = $$PWD/com.gitee.wanywhn.EveryLauncher.xml
dbus.header_flags += -l DBusProxy -i $$PWD/dbusproxy.h
dbus.source_flags += -l DBusProxy

dbus_itface.files= $$PWD/com.gitee.wanywhn.EveryLauncher.xml
dbus_itface.header_flags += -c EveryLauncherInterface
dbus_itface.source_flags += -c EveryLauncherInterface

DBUS_ADAPTORS += dbus
DBUS_INTERFACES +=dbus_itface

target.path = $$PREFIX/bin

dbus_xmls.files = $$dbus.files
dbus_xmls.path = /usr/share/dbus-1/interfaces

INSTALLS += target dbus_xmls

TEMPLATE=subdirs
SUBDIRS +=src


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


isEmpty(PREFIX): PREFIX = /usr

dbus_service.files = $$PWD/dbus/com.gitee.wanywhn.EveryLauncher.service
dbus_service.path = /usr/share/dbus-1/services

dbus_toggle.files=$$PWD/bin/everylauncher-toggle
dbus_toggle.path=$$PREFIX/bin


deplib.files=$$PWD/lib
deplib.path=$${PREFIX}/

desktop.files=$$PWD/EveryLauncher.desktop
desktop.path=$${PREFIX}/share/applications/

desktopicon.files=$$PWD/everylauncher.svg
desktopicon.path=$$PREFIX/share/icons/hicolor/scalable/apps

recoll_conf.files=$$PWD/recoll_conf
recoll_conf.path=$$PREFIX/share/everylauncher/

filters.files=$$PWD/filters
filters.path=$$PREFIX/share/everylauncher/

icon.files=$$PWD/icon
icon.path=$$PREFIX/share/everylauncher/

INSTALLS += dbus_service dbus_toggle deplib desktop recoll_conf filters icon desktopicon

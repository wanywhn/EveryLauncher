#include <QApplication>
#include <QDesktopWidget>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QString>

#include "widget.h"
#include "config.h"
#include "systemtray.h"

QString AppName="EveryLauncher";
QString RECOLL_CONFIG_DIR="recoll";
QString RECOLL_CONFIG_FILE="recoll.conf";
QString XAPIAN_DB_DIR="xapiandb";
QString DBUS_SERVICE="com.gitee.wanywhn.EveryLauncher";
QString DBUS_PATH="/com/gitee/wanywhn/EveryLauncher";
QString DBUS_INTERFACE="com.gitee.wanywhn.EveryLauncher";
QString ORGANIZATION_NAME="WANYWHN";

void _create_dirs(){
    auto cfgDir=QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
    if(!cfgDir.exists()){
        cfgDir.mkpath(cfgDir.absolutePath());
    }
    QDir::setCurrent(cfgDir.absolutePath());
    cfgDir.setPath(RECOLL_CONFIG_DIR);
    if(!cfgDir.exists()){
        cfgDir.mkdir(cfgDir.absolutePath());
    }
    QDir::setCurrent(cfgDir.absolutePath());
    cfgDir.setPath(XAPIAN_DB_DIR);
    if(!cfgDir.exists()){
        cfgDir.mkdir(cfgDir.absolutePath());
    }
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    a.setApplicationName(AppName);
    _create_dirs();

    auto conn=QDBusConnection::sessionBus();
    if(!conn.isConnected()){
        return -1;
    }
    if(!conn.registerService(DBUS_SERVICE)){
        auto itface=new QDBusInterface(DBUS_SERVICE,DBUS_PATH,DBUS_INTERFACE);
        itface->call("showWindow");
        return 0;
    }else{
        //TODO register object
    }
    Widget w;
    auto desktop=QApplication::desktop();
    SystemTray::getInstance(&w).show();
    QObject::connect(&SystemTray::getInstance(&w),&SystemTray::exitAll,[](){
        qApp->exit();
    });

    qApp;
    w.move((desktop->width()-w.width())/2,(desktop->height()-w.height())/3);
    w.show();

    return a.exec();
}

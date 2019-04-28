#include <QApplication>
#include <QDesktopWidget>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QString>
#include <QMessageBox>

#include "widget.h"
#include "config.h"
#include "systemtray.h"
#include "rclinit.h"

QString AppName="EveryLauncher";
QString RECOLL_CONFIG_DIR="recoll";
QString RECOLL_CONFIG_FILE="recoll.conf";
QString XAPIAN_DB_DIR="xapiandb";
QString DBUS_SERVICE="com.gitee.wanywhn.EveryLauncher";
QString DBUS_PATH="/com/gitee/wanywhn/EveryLauncher";
QString DBUS_INTERFACE="com.gitee.wanywhn.EveryLauncher";
QString ORGANIZATION_NAME="WANYWHN";

RclConfig *theconfig;
std::shared_ptr<Rcl::Db> rcldb;
bool maybeOpenDb(string &reason, bool force, bool *maindberror)
{
//    LOGDEB2("maybeOpenDb: force " << force << "\n");

    if (force) {
        rcldb = std::shared_ptr<Rcl::Db>(new Rcl::Db(theconfig));
    }
    rcldb->rmQueryDb("");
//    for (const auto& dbdir : prefs.activeExtraDbs) {
//        LOGDEB("main: adding [" << dbdir << "]\n");
//        rcldb->addQueryDb(dbdir);
//    }
    Rcl::Db::OpenError error;
    if (!rcldb->isopen() && !rcldb->open(Rcl::Db::DbRO, &error)) {
        reason = "Could not open database";
        if (maindberror) {
            reason +=  " in " +  theconfig->getDbDir() +
                " wait for indexing to complete?";
            *maindberror = (error == Rcl::Db::DbOpenMainDb) ? true : false;
        }
        return false;
    }
//    rcldb->setAbstractParams(-1, prefs.syntAbsLen, prefs.syntAbsCtx);
    return true;
}
// This is never called because we _Exit() in rclmain_w.cpp
static void recollCleanup()
{
//    LOGDEB2("recollCleanup: closing database\n" );
    rcldb.reset();
    deleteZ(theconfig);

//    deleteAllTempFiles();
//    LOGDEB2("recollCleanup: done\n" );
}
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

    std::string reason;
    //TODO -c
    std::string confg="/home/tender/.config/EveryLauncher/recoll";
    theconfig = recollinit(0, recollCleanup, 0, reason,&confg);
    if (!theconfig || !theconfig->ok()) {
        QString msg = "Configuration problem: ";
        msg += QString::fromUtf8(reason.c_str());
        QMessageBox::critical(0, "Recoll",  msg);
        exit(1);
    }
    bool b;
    maybeOpenDb(reason,1,&b);
    //    fprintf(stderr, "recollinit done\n");
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
    SystemTray::getInstance(&w).show();
    QObject::connect(&SystemTray::getInstance(&w),&SystemTray::exitAll,[](){
        qApp->exit();
    });

    auto desktop=QApplication::desktop();
    w.move((desktop->width()-w.width())/2,(desktop->height()-w.height())/3);
    w.show();

    return a.exec();
}
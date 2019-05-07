#include <memory>
#include <DApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDebug>
#include <QDesktopWidget>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>
#include <QString>
#include <DTitlebar>
#include <XdgDirs>
#include <QProcess>

#include "config.h"
#include "dbusproxy.h"
#include "everylauncher_adaptor.h"
#include "everylauncher_interface.h"
#include "everylaunchermonitor_interface.h"
#include "keymonitor.h"
#include "rclinit.h"
#include "systemtray.h"
#include "widget.h"

QString AppName = "EveryLauncher";
QString RECOLL_CONFIG_DIR = "recoll_conf";
QString RECOLL_CONFIG_FILE = "recoll.conf";
QString XAPIAN_DB_DIR = "xapiandb";
QString DBUS_SERVICE = "com.gitee.wanywhn.EveryLauncher";
QString DBUS_PATH = "/com/gitee/wanywhn/EveryLauncher";
// QString DBUS_INTERFACE="com.gitee.wanywhn.EveryLauncher";
QString ORGANIZATION_NAME = "WANYWHN";

#define DBUS_MONITOR_SERVER "com.gitee.wanywhn.everylauncherMonitor"
#define DBUS_MONITOR_PATH "/com/gitee/wanywhn/everylauncherMonitor"
//#define DBUS_INTERFACE "com.gitee.wanywhn.everylauncherMonitor"
RclConfig *theconfig;
std::shared_ptr<Rcl::Db> rcldb;

/**
 * 打开数据库
 * @param reason 传出错误用参数
 * @param force 是否强制重新打开
 * @param maindberror 是否返回是主数据库打开错误
 * @return
 */
bool maybeOpenDb(string &reason, bool force, bool *maindberror) {

    if (force) {
        rcldb = std::make_shared<Rcl::Db>(theconfig);
    }
    rcldb->rmQueryDb("");
    Rcl::Db::OpenError error;
    if (!rcldb->isopen() && !rcldb->open(Rcl::Db::DbRO, &error)) {
        reason = "Could not open database";
        if (maindberror) {
            reason +=
                    " in " + theconfig->getDbDir() + " wait for indexing to complete?";
            *maindberror = error == Rcl::Db::DbOpenMainDb;
        }
        return false;
    }
    return true;
}

static void recollCleanup() {
    rcldb.reset();
    deleteZ(theconfig);
}

void _create_dirs() {
    auto cfgDir =
            QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
    if (!cfgDir.exists()) {
        cfgDir.mkpath(cfgDir.absolutePath());
    }
    //TODO clear filter
        if(!cfgDir.exists(RECOLL_CONFIG_DIR)){
        QStringList args;
        args<<"/usr/share/everylauncher/recoll_conf";
        args<<cfgDir.absolutePath();
        args<<"-r";
        QProcess::execute("cp",args);
        }

        //TODO clear db
    cfgDir.setPath(RECOLL_CONFIG_DIR);
    if (!cfgDir.exists(XAPIAN_DB_DIR)) {
        cfgDir.mkdir(XAPIAN_DB_DIR);
    }
}

int main(int argc, char *argv[]) {
    DApplication::loadDXcbPlugin();
    DApplication a(argc, argv);
    DApplication::setApplicationName("EveryLauncher");
    DApplication::setOrganizationName("EveryLauncher");
    Dtk::Widget::DApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    Dtk::Widget::DApplication::setQuitOnLastWindowClosed(false);
    Dtk::Widget::DApplication::setApplicationName(AppName);
    _create_dirs();

    MainWindow w;


    std::string reason;
    // TODO -c
    auto cfh=QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)).absoluteFilePath(RECOLL_CONFIG_DIR);
    std::string confg =cfh.toStdString();
    theconfig = recollinit(0, recollCleanup, nullptr, reason, &confg);
    if (!theconfig || !theconfig->ok()) {
        QString msg = "Configuration problem: ";
        msg += QString::fromUtf8(reason.c_str());
        QMessageBox::critical(nullptr, "Recoll", msg);
        exit(1);
    }
    bool b;
    maybeOpenDb(reason, 1, &b);
    //    fprintf(stderr, "recollinit done\n");
    auto conn = QDBusConnection::sessionBus();
    if (!conn.isConnected()) {
        return -1;
    }
//  w.setWindowOpacity(0.1);
//  w.setTranslucentBackground(true);
//  w.setAttribute(Qt::WA_TranslucentBackground);
    w.setEnableBlurWindow(true);

//  w.setWindowFlags(Qt::FramelessWindowHint);

    DBusProxy proxy(SystemTray::getInstance(&w), w);
    EveryLauncherAdaptor adaptor(&proxy);
    if (!conn.registerService(DBUS_SERVICE)) {
        EveryLauncherInterface itface(DBUS_SERVICE, DBUS_PATH, conn);
        itface.showWindow();
        return 0;
    } else {
        conn.registerObject(DBUS_PATH, &proxy);
    }
    EveryLauncherMonitorInterface monitorItfc(
            DBUS_MONITOR_SERVER, DBUS_MONITOR_PATH, QDBusConnection::sessionBus());
    QObject::connect(&monitorItfc, &EveryLauncherMonitorInterface::fileWrited,
                     [](QStringList sl) { qDebug() << "get?" << sl; });
    SystemTray::getInstance(&w).show();
    QObject::connect(&SystemTray::getInstance(&w), &SystemTray::exitAll,
                     []() { qApp->exit(); });
    auto desktop = QApplication::desktop();
    w.move((desktop->width() - w.width()) / 2,
           (desktop->height() - w.height()) / 3);

    auto fixdwid = desktop->width() / 1.5;
    fixdwid = fixdwid > 1200 ? 1200 : fixdwid;
    auto fixhei = desktop->height() / 1.5;
    fixhei = fixhei > 1000 ? 1000 : fixhei;

    w.setMinimumSize(fixdwid, fixhei);
    w.setMaximumSize(fixdwid, fixhei);
    w.show();

    return Dtk::Widget::DApplication::exec();
}

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
#include "firsttimeinit.h"
#include "keymonitor.h"
#include "rclinit.h"
#include "systemtray.h"
#include "widget.h"

const QString AppName = "EveryLauncher";
const QString RECOLL_CONFIG_DIR = "recoll_conf";
const QString RECOLL_CONFIG_FILE = "recoll.conf";
const QString XAPIAN_DB_DIR = "xapiandb";
const QString DBUS_SERVICE = "com.gitee.wanywhn.EveryLauncher";
const QString DBUS_PATH = "/com/gitee/wanywhn/EveryLauncher";
// QString DBUS_INTERFACE="com.gitee.wanywhn.EveryLauncher";
const QString ORGANIZATION_NAME = "WANYWHN";

const QString DBUS_MONITOR_SERVER ="com.gitee.wanywhn.EveryLauncherMonitor";
const QString DBUS_MONITOR_PATH ="/com/gitee/wanywhn/EveryLauncherMonitor";
//#define DBUS_INTERFACE "com.gitee.wanywhn.everylauncherMonitor"
RclConfig *theconfig;
std::shared_ptr<Rcl::Db> rcldb;

static QMap<QString,QStringList> searchEngineMap={
    {"so",{"http://stackoverflow.com/search?q=","StackOverflow"}},
    {"bi",{"https://cn.bing.com/search?q=","Bing"}},
    {"zh",{"http://www.zhihu.com/search?q=","知乎"}},
    {"bd",{"https://www.baidu.com/s?wd=","百度"}},

};
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
    DApplication::setApplicationName(AppName);
    DApplication::setOrganizationName(AppName);
    DApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    DApplication::setQuitOnLastWindowClosed(false);
//    Dtk::Widget::DApplication::setApplicationName(AppName);
    _create_dirs();



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
    MainWindow w;
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
//    QObject::connect(&monitorItfc, &EveryLauncherMonitorInterface::fileWrited,
//                     [](QStringList sl) { qDebug() << "get?" << sl;
//                                        });
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
    QSettings s;
    if(s.value("firstTime",true).toBool()){
        firstTimeInit ftDialog(&w);
        if(ftDialog.exec()!=QDialog::Accepted){
            QMessageBox::warning(&w,QObject::tr("尚未索引"),QObject::tr("警告，尚未创建索引。可能无法搜索到东西。"));
        }else{
            s.setValue("firstTime",false);

        }
    }

    QSettings sett;
    sett.beginGroup("SearchPrefix");
    for(auto item:searchEngineMap.keys()){
        if(!sett.contains(item)){
            sett.setValue(item,searchEngineMap.value(item));
        }
    }

    sett.endGroup();
    return Dtk::Widget::DApplication::exec();
}

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
#include <QtWebView/QtWebView>
#include <DAboutDialog>

#include "config.h"
#include "dbusproxy.h"
#include "everylauncher_adaptor.h"
#include "everylauncher_interface.h"
#include "everylaunchermonitor_interface.h"
#include "firsttimeinit.h"
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

const QString DBUS_MONITOR_SERVER = "com.gitee.wanywhn.EveryLauncherMonitor";
const QString DBUS_MONITOR_PATH = "/com/gitee/wanywhn/EveryLauncherMonitor";
//#define DBUS_INTERFACE "com.gitee.wanywhn.everylauncherMonitor"
RclConfig *theconfig;
std::shared_ptr<Rcl::Db> rcldb;

static QMap<QString, QStringList> searchEngineMap = {
        {"so", {"http://stackoverflow.com/search?q=", "StackOverflow"}},
        {"bi", {"https://cn.bing.com/search?q=",      "必应"}},
        {"zh", {"http://www.zhihu.com/search?q=",     "知乎"}},
        {"bd", {"https://www.baidu.com/s?wd=",        "百度"}},
        {"gg", {"https://www.google.com.hk/search?q=",        "谷歌"}},

};

void create_deepin_sk();

void create_search_engine();

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
    if (!cfgDir.exists(RECOLL_CONFIG_DIR)) {
        QStringList args;
        args << "/usr/share/everylauncher/recoll_conf";
        args << cfgDir.absolutePath();
        args << "-r";
        QProcess::execute("cp", args);
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
    _create_dirs();


    std::string reason;
    auto cfh = QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)).absoluteFilePath(
            RECOLL_CONFIG_DIR);
    std::string confg = cfh.toStdString();
    theconfig = recollinit(0, recollCleanup, nullptr, reason, &confg);
    if (!theconfig || !theconfig->ok()) {
        QString msg = "Configuration problem: ";
        msg += QString::fromUtf8(reason.c_str());
        QMessageBox::critical(nullptr, "Recoll", msg);
        exit(1);
    }
    bool b;
    maybeOpenDb(reason, true, &b);
    auto conn = QDBusConnection::sessionBus();
    if (!conn.isConnected()) {
        return -1;
    }
    MainWindow w;
    QIcon icon(":/icon/everylauncher.svg");
    DApplication::setWindowIcon(icon);
    DApplication::setApplicationDisplayName(QObject::tr("Everylayncher"));
    DApplication::setDesktopFileName("EveryLauncher.desktop");
    DApplication::setApplicationVersion(E_VERSION);
    a.setProductIcon(icon);
    auto abt=new DAboutDialog(&w);
    abt->setWebsiteName("https://github.com/wanywhn/EveryLauncher");
    abt->setWebsiteLink("https://github.com/wanywhn/EveryLauncher");
    abt->setCompanyLogo(icon.pixmap(64));
    abt->setWindowIcon(icon);
    abt->setDescription(QObject::tr("一款检索程序"));
    abt->setAcknowledgementVisible(false);
    abt->setVersion(E_VERSION);

    a.setAboutDialog(abt);
    w.setWindowIcon(icon);
    w.setEnableBlurWindow(true);

//    w.setWindowFlags(Qt::FramelessWindowHint);

//    DBusProxy proxy(&w);
    auto proxy=DBusProxy::getInstance();
    EveryLauncherAdaptor adaptor(proxy);
    if (!conn.registerService(DBUS_SERVICE)) {
        ComGiteeWanywhnEveryLauncherInterface itface(DBUS_SERVICE, DBUS_PATH, conn);
        itface.showWindow();
        return 0;
    } else {
        conn.registerObject(DBUS_PATH, proxy);
    }
    ComGiteeWanywhnEveryLauncherMonitorInterface interface(DBUS_MONITOR_SERVER,DBUS_MONITOR_PATH,conn);
    if(!interface.isValid()){
        interface.wakeup();
    }
//                                        });
    SystemTray::getInstance(&w).show();
    SystemTray::getInstance(&w).setIcon(icon);
    QObject::connect(&SystemTray::getInstance(&w), &SystemTray::exitAll,
                     []() { qApp->exit(); });

    auto desktop = QApplication::desktop();

    auto fixdwid = desktop->width() / 1.3;
    fixdwid = fixdwid > 1400 ? 1400 : fixdwid;
    auto fixhei = desktop->height() / 1.2;
    fixhei = fixhei > 1400 ? 1400 : fixhei;

    w.setMinimumSize(fixdwid, fixhei);
    w.setMaximumSize(fixdwid, fixhei);

    w.move((desktop->width() - w.width()) / 2,
           (desktop->height() - w.height()) / 3);

    w.show();

    QSettings s;
    if (s.value("firstTime", true).toBool()) {
        create_deepin_sk();
        create_search_engine();
        firstTimeInit ftDialog(&w);
        if (ftDialog.exec() != QDialog::Accepted) {
            QMessageBox::warning(&w, QObject::tr("尚未索引"), QObject::tr("警告，尚未创建索引。可能无法搜索到东西。"));
        } else {
            s.setValue("firstTime", false);

        }
    }

    return DApplication::exec();
}

void create_search_engine() {
    QSettings sett;
    sett.setIniCodec("UTF-8");
    sett.beginGroup("SearchPrefix");
    for (const auto &item:searchEngineMap.keys()) {
        if (!sett.contains(item)) {
            sett.setValue(item, searchEngineMap.value(item));
        }
    }

    sett.endGroup();
}

void create_deepin_sk() {
    //!!FIXME 中文
    auto customIni=QDir::home().absoluteFilePath(".config/deepin/dde-daemon/keybinding/custom.ini");

    qDebug()<<"ini file:"<<customIni;
    QSettings s(customIni,QSettings::Format::IniFormat);
    s.setIniCodec("UTF-8");
    auto cg=s.childGroups();
    bool finded=false;
    for(const auto &g:cg){
        s.beginGroup(g);
        if(s.value("Name")==AppName){
            finded=true;
            break;
        }
        s.endGroup();
    }
    if(!finded){
        QUuid id=QUuid::createUuid();
        while (cg.contains(id.toString())){
            id=QUuid::createUuid();
        }
        s.beginGroup(id.toString().remove("{").remove("}"));
        s.setValue("Name",AppName);
        s.setValue("Action","everylauncher-toggle");
        s.setValue("Accels","<Alt><Super>space");

        s.endGroup();
        QDBusInterface keybings(
                "com.deepin.daemon.Keybinding",
                "/com/deepin/daemon/Keybinding",
                "com.deepin.daemon.Keybinding"
                );
        keybings.call("Reset");
    }
}

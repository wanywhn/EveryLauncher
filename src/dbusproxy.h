#ifndef DBUSPROXY_H
#define DBUSPROXY_H

#include "everylaunchermonitor_interface.h"
#include "systemtray.h"
#include "widget.h"
#include "config.h"

#include <QObject>

class MonitorFactory{
public:
    static EveryLauncherMonitorInterface *getInstance(){
        static auto instance=new EveryLauncherMonitorInterface(
                    DBUS_MONITOR_SERVER,DBUS_MONITOR_PATH,
                    QDBusConnection::sessionBus());
        return instance;
    }
private:
//    EveryLauncherMonitorInterface monitorItfc(
//            DBUS_MONITOR_SERVER, DBUS_MONITOR_PATH, QDBusConnection::sessionBus());
};
class DBusProxy : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface","com.gitee.wanywhn.EveryLauncher")
public:
    explicit DBusProxy(QObject *parent = nullptr);
    static DBusProxy *getInstance(){
        static DBusProxy dBusProxy;
        return &dBusProxy;
    }

public slots:
    void IndexChangeFiles(QStringList paths);
    void showWindow();

signals:
    void mShowWindow();
    void mIndexChangeFiles(QStringList paths);

private:

};

#endif // DBUSPROXY_H

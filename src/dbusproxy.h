#ifndef DBUSPROXY_H
#define DBUSPROXY_H

#include "systemtray.h"
#include "widget.h"

#include <QObject>

class DBusProxy : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface","com.gitee.wanywhn.EveryLauncher")
public:
    explicit DBusProxy(SystemTray &t,Widget &w,QObject *parent = nullptr);

public slots:
    void IndexChangeFiles(QStringList paths);
    void showWindow();

signals:

private:
    SystemTray &tray;
    Widget 	&widget;

};

#endif // DBUSPROXY_H

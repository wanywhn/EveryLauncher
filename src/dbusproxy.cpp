#include <utility>

#include "dbusproxy.h"

DBusProxy::DBusProxy(QObject *parent)
{

}

void DBusProxy::IndexChangeFiles(QStringList paths)
{
    //TODO start Index
    emit mIndexChangeFiles(paths);

}

void DBusProxy::showWindow()
{
//    this->tray.showWindow();
emit mShowWindow();

}

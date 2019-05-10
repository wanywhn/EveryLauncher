#include <utility>

#include "dbusproxy.h"

DBusProxy::DBusProxy(SystemTray &t, MainWindow &w, QObject *parent):tray(t),widget(w)
{

}

void DBusProxy::IndexChangeFiles(QStringList paths)
{
    //TODO start Index
    qDebug()<<"here";
    this->widget.IndexSomeFiles(std::move(paths));

}

void DBusProxy::showWindow()
{
    //TODO
    this->tray.showWindow();

}

#include "systemtray.h"
#include "preferencewindow.h"
#include <QDebug>
#include <QIcon>
#include "confgui/confguiindex.h"
#include "dbusproxy.h"
#include <QtWidgets/QApplication>

SystemTray::SystemTray(QWidget *mpranet)
    : QSystemTrayIcon(), mainWindow(mpranet) {

  init_ui();
  init_conn();
  connect(DBusProxy::getInstance(),&DBusProxy::mShowWindow,this,&SystemTray::showWindow);
}

void SystemTray::showWindow() {
    if(this->mainWindow->isActiveWindow()){
        this->mainWindow->hide();
    }else{
        this->mainWindow->raise();
        this->mainWindow->show();
  this->mainWindow->activateWindow();
  QApplication::setActiveWindow(this->mainWindow);
    }
}

void SystemTray::init_ui() {

  this->trayMenu = new QMenu();
  this->showUp = new QAction(tr("显示主窗口"), this->trayMenu);
  this->preferenceAction = new QAction(tr("选项"), this->trayMenu);
  this->exitAction = new QAction(tr("退出"), this->trayMenu);

  this->trayMenu->addAction(this->showUp);
  this->trayMenu->addAction(this->preferenceAction);
  this->trayMenu->addAction(this->exitAction);

  this->setContextMenu(this->trayMenu);
}

void SystemTray::init_conn() {

  connect(this->showUp, &QAction::triggered, this,
          &SystemTray::showWindow); //[this](){
  connect(this->preferenceAction, &QAction::triggered, [this]() {
    ELconfgui::ConfIndexW w(nullptr, theconfig);
    w.exec();
    emit ConfigChanged();
  });

  connect(this->exitAction, &QAction::triggered, this, &SystemTray::exitAll);
}

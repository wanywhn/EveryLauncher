#include "preferencewindow.h"
#include "systemtray.h"
#include <QIcon>
#include <QDebug>

SystemTray::SystemTray(QWidget *mpranet):QSystemTrayIcon(mpranet),mainWindow(mpranet)
{

    init_ui();


    init_conn();
}

void SystemTray::init_ui()
{

    this->trayMenu=new QMenu();
    this->showUp=new QAction(tr("ShowMainWindow"),this->trayMenu);
    this->preferenceAction=new QAction(tr("Preference"),this->trayMenu);
    this->exitAction=new QAction(tr("Exit"),this->trayMenu);

    this->trayMenu->addAction(this->showUp);
    this->trayMenu->addAction(this->preferenceAction);
    this->trayMenu->addAction(this->exitAction);

    this->setContextMenu(this->trayMenu);

}

void SystemTray::init_conn()
{

    connect(this->showUp,&QAction::triggered,[this](){
        if(!this->mainWindow->isVisible()){
            this->mainWindow->show();
            this->mainWindow->activateWindow();
        }else if(!this->mainWindow->isActiveWindow()){
            this->mainWindow->activateWindow();
            this->mainWindow->raise();
        }else{
            this->mainWindow->hide();
        }
//        if(this->mainWindow->isVisible()){
//            this->mainWindow->hide();
//        }
    });
    connect(this->preferenceAction,&QAction::triggered,[this](){
        PreferenceWindow pw(this->mainWindow);
        pw.exec();

    });

    connect(this->exitAction,&QAction::triggered,this,&SystemTray::exitAll);

}

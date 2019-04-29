#include "confguiindex.h"
#include "preferencewindow.h"
#include "systemtray.h"
#include <QIcon>
#include <QDebug>

SystemTray::SystemTray(QWidget *mpranet):QSystemTrayIcon(),mainWindow(mpranet)
{

    init_ui();


    init_conn();
}

void SystemTray::showWindow()
{
    if(this->mainWindow->isActiveWindow()){
        this->mainWindow->hide();
        return ;
    }
    if(this->mainWindow->isVisible()&&!this->mainWindow->isHidden()){
        qDebug()<<"heler";
        this->mainWindow->show();
//        this->mainWindow->setWindowFlags(this->mainWindow->windowFlags()|Qt::WindowStaysOnTopHint);
//        this->mainWindow->setWindowFlag(Qt::WindowStaysOnTopHint,false);
        this->mainWindow->raise();
        this->mainWindow->activateWindow();
//        this->mainWindow->hide();
        return ;
    }
    qDebug()<<"agen";

    //FIXME 貌似需要量下才能现实？
//    this->mainWindow->hide();
//    this->trayMenu->hideTearOffMenu();
//    this->trayMenu->showTearOffMenu();
//    this->hide();
//    this->trayMenu->show();
        this->mainWindow->show();
    this->mainWindow->raise();
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

    connect(this->showUp,&QAction::triggered,this,&SystemTray::showWindow);//[this](){
    connect(this->preferenceAction,&QAction::triggered,[](){
//        PreferenceWindow pw(this->mainWindow);

        confgui::ConfIndexW w(nullptr,theconfig);
        w.exec();

    });

    connect(this->exitAction,&QAction::triggered,this,&SystemTray::exitAll);

}

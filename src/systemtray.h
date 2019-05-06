#ifndef SYSTEMTRAY_H
#define SYSTEMTRAY_H

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>
#include <rclconfig.h>

extern RclConfig *theconfig;
class SystemTray : public QSystemTrayIcon
{
    Q_OBJECT
public:
    explicit SystemTray(QWidget *mpranet);
    static SystemTray& getInstance(QWidget *parent){
        static SystemTray instance(parent);
        return instance;

    }
public slots:
    void showWindow();
private:
    void init_ui();
    void init_conn();

 signals:
    void exitAll();
private:
    QWidget *mainWindow;
    QMenu *trayMenu;
    QAction *showUp;
    QAction *preferenceAction;
    QAction *exitAction;
};

#endif // SYSTEMTRAY_H

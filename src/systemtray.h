#ifndef SYSTEMTRAY_H
#define SYSTEMTRAY_H

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>

class SystemTray : public QSystemTrayIcon
{
    Q_OBJECT
public:
    SystemTray(QWidget *mpranet);
    static SystemTray& getInstance(QWidget *parent){
        static SystemTray instance(parent);
        return instance;

    }
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

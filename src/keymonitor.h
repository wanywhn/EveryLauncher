#ifndef KEYMONITOR_H
#define KEYMONITOR_H

#include <QObject>
#include <QThread>
#include <X11/Xlib.h>
#include <X11/extensions/record.h>

class EventMonitor : public QThread
{
    Q_OBJECT

public:
    EventMonitor(QObject *parent = 0);

signals:
    void buttonPress(int x, int y);
    void buttonDrag(int x, int y);
    void buttonRelease(int x, int y);
    void keyPress(int code);
    void keyRelease(int code);

protected:
    bool filterWheelEvent(int detail);
    static void callback(XPointer trash, XRecordInterceptData* data);
    void handleRecordEvent(XRecordInterceptData *);
    void run();

private:
    bool isPress;
};
#endif // KEYMONITOR_H

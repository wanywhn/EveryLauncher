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
    void hotKeyPressed();

protected:
    static void callback(XPointer trash, XRecordInterceptData* data);
    void handleRecordEvent(XRecordInterceptData *);
    void run();

private:
    bool metaisPress{false};
    bool spaceisPress{false};

};
#endif // KEYMONITOR_H

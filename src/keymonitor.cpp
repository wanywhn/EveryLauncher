#include "keymonitor.h"
#include <X11/Xlibint.h>

EventMonitor::EventMonitor(QObject *parent) : QThread(parent)
{
}

void EventMonitor::run()
{
    Display* display = XOpenDisplay(nullptr);
    if (display == nullptr) {
        fprintf(stderr, "unable to open display\n");
        return;
    }

    // Receive from ALL clients, including future clients.
    XRecordClientSpec clients = XRecordAllClients;
    XRecordRange* range = XRecordAllocRange();
    if (range == nullptr) {
        fprintf(stderr, "unable to allocate XRecordRange\n");
        return;
    }

    // Receive KeyPress, KeyRelease
    memset(range, 0, sizeof(XRecordRange));
    range->device_events.first = KeyPress;
    range->device_events.last  = KeyRelease;

    // And create the XRECORD context.
    XRecordContext context = XRecordCreateContext(display, 0, &clients, 1, &range, 1);
    if (context == 0) {
        fprintf(stderr, "XRecordCreateContext failed\n");
        return;
    }
    XFree(range);

    XSync(display, True);

    Display* display_datalink = XOpenDisplay(0);
    if (display_datalink == 0) {
        fprintf(stderr, "unable to open second display\n");
        return;
    }

    if (!XRecordEnableContext(display_datalink, context,  callback, (XPointer) this)) {
        fprintf(stderr, "XRecordEnableContext() failed\n");
        return;
    }
}

void EventMonitor::callback(XPointer ptr, XRecordInterceptData* data)
{
    ((EventMonitor *) ptr)->handleRecordEvent(data);
}

void EventMonitor::handleRecordEvent(XRecordInterceptData* data)
{
    if (data->category == XRecordFromServer) {
        xEvent * event = (xEvent *)data->data;
        switch (event->u.u.type) {
        case KeyPress:
            if((((unsigned char*)data->data))[1]=133){
                metaisPress=true;
            }
            if((((unsigned char*)data->data))[1]=65){
                spaceisPress=true;
            }
            if(spaceisPress&&metaisPress){
                emit hotKeyPressed();
            }

            break;
        case KeyRelease:
            if((((unsigned char*)data->data))[1]=133){
                metaisPress=false;
            }
            if((((unsigned char*)data->data))[1]=65){
                spaceisPress=false;
            }

            break;
        default:
            break;
        }
    }

    fflush(stdout);
    XRecordFreeData(data);
}

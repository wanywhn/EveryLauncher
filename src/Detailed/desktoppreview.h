#ifndef DESKTOPPREVIEW_H
#define DESKTOPPREVIEW_H

#include "detailedtext.h"

#include <QLabel>
#include <QObject>

class DesktopPreview : public DetailedW
{
public:
    DesktopPreview();

    // DetailedW interface
public:
    void showDoc(Rcl::Doc doc) override;


private:
    void init_ui();
    QLabel  *icon;
    QLabel	*appName;
    QLabel *appInstalledTime;
    QLabel *appSize;
    QLabel	*comment;

};

#endif // DESKTOPPREVIEW_H

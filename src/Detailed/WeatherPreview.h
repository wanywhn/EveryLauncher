//
// Created by tender on 19-5-24.
//

#ifndef EVERYLAUNCHER_WEATHERPREVIEW_H
#define EVERYLAUNCHER_WEATHERPREVIEW_H


#include <QtWebEngineWidgets/QWebEngineView>
#include "detailedtext.h"
#include <QNetworkReply>

class WeatherPreview: public DetailedW {
public:
    WeatherPreview();

    void showDoc() override;

public slots:
    void processHtml(QNetworkReply *rpl);

private:
    QWebEngineView *view;
};


#endif //EVERYLAUNCHER_WEATHERPREVIEW_H

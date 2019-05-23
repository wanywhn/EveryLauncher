//
// Created by tender on 19-5-23.
//

#ifndef EVERYLAUNCHER_MAP_PREVIEW_H
#define EVERYLAUNCHER_MAP_PREVIEW_H


#include <QtWebEngineWidgets/QWebEngineView>
#include "detailedtext.h"

class map_preview : public DetailedW{
public:
    map_preview();

    void showDoc() override;

private:
    QWebEngineView *view;
};


#endif //EVERYLAUNCHER_MAP_PREVIEW_H

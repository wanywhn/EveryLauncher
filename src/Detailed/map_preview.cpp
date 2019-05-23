//
// Created by tender on 19-5-23.
//

#include <QtWidgets/QVBoxLayout>
#include <Model/ELModelInterface.h>
#include "map_preview.h"

void map_preview::showDoc() {
    auto url=index.data(ELModelInterface::ModelRoles::Role_MAP_URL).toString();
    view->page()->load(QUrl(url));
}

map_preview::map_preview() {
    setSupportedMimeType={
            MAP_TYPE.toStdString(),
    };
    view=new QWebEngineView(this);

    auto vlayout=new QVBoxLayout();
    vlayout->addWidget(view);
    this->setLayout(vlayout);

}

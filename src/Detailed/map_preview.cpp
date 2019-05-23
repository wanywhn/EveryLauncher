//
// Created by tender on 19-5-23.
//

#include <QtWidgets/QVBoxLayout>
#include <Model/ELModelInterface.h>
#include <QtWidgets/QMessageBox>
#include "map_preview.h"

void map_preview::showDoc() {
    auto url=index.data(ELModelInterface::ModelRoles::Role_MAP_URL).toString();
    auto qurl=QUrl("qrc:/html/GaoDeMap.html");
    view->page()->load(qurl);
//    view->setContent(html.toLocal8Bit(),"text/html");

//    view->page()->load(QUrl(url));
}

map_preview::map_preview() {
    setSupportedMimeType={
            MAP_TYPE.toStdString(),
    };
    view=new QWebEngineView(this);

    auto vlayout=new QVBoxLayout();
    vlayout->addWidget(view);
    this->setLayout(vlayout);

    auto page=view->page();
    connect(page, &QWebEnginePage::featurePermissionRequested,
            [this, page](const QUrl &securityOrigin, QWebEnginePage::Feature feature) {
//                if (feature != QWebEnginePage::Geolocation)
//                    return;

                QMessageBox msgBox(this);
                msgBox.setText(tr("%1 wants to know your location").arg(securityOrigin.host()));
                msgBox.setInformativeText(tr("Do you want to send your current location to this website?"));
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::Yes);

                if (msgBox.exec() == QMessageBox::Yes) {
                    page->setFeaturePermission(
                            securityOrigin, feature, QWebEnginePage::PermissionGrantedByUser);
                } else {
                    page->setFeaturePermission(
                            securityOrigin, feature, QWebEnginePage::PermissionDeniedByUser);
                }
            });

}

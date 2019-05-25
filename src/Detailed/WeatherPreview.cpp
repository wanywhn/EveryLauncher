//
// Created by tender on 19-5-24.
//

#include <QtWidgets/QVBoxLayout>
#include "WeatherPreview.h"
#include <QWebEnginePage>
#include "Model/ELModelInterface.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QtXml/QDomDocument>

void WeatherPreview::showDoc() {
    auto manager=new QNetworkAccessManager();
    QNetworkRequest request;
    request.setUrl(QUrl("https://www.so.com/s?ie=utf-8&fr=so.360.cn&src=home_so.360.cn&q=%E5%A4%A9%E6%B0%94"));
    request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader,"Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36");
//    request.setRawHeader("Accept-Encoding","gzip, deflate");
    request.setRawHeader("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8");
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    request.setRawHeader("Accept-Language","zh-CN,zh;q=0.9");
    request.setRawHeader("Connection","keep-alive");
//    request.setRawHeader("User-Agent","Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36");
    manager->get(request);
    connect(manager,&QNetworkAccessManager::finished,this,&WeatherPreview::processHtml);
//    view->page()->load(QUrl("qrc:/html/weather.html"));
}

WeatherPreview::WeatherPreview() {
    setSupportedMimeType={
            WEATHER_TYPE.toStdString(),
    };
    view=new QWebEngineView;
    auto vlayout=new QVBoxLayout();
    this->setLayout(vlayout);
    vlayout->addWidget(view);
}

void WeatherPreview::processHtml(QNetworkReply *rpl) {
        QTextStream qts(rpl->readAll());
    QDomDocument doc;
    doc.setContent(qts.readAll());
    QStringList sl;
    sl<<"res_news_flow";
    sl<<"side";
    sl<<"footer";
    sl<<"header";
    sl<<"tabs-wrap";
//    for(auto i:sl){
//        auto elemelt=doc.elementById(i);
//        doc.removeChild(elemelt);
//    }
view->page()->setContent(doc.toByteArray());
//    view->page()->setHtml(doc.toString());
}

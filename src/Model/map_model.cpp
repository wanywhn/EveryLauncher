//
// Created by tender on 19-5-23.
//

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QtCore/QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QTimer>

#include "map_model.h"

void map_model::search(std::string &string) {
    mapIcon.clear();

    auto url=restUrl.arg("125.31364243,43.89833761").arg(QString::fromStdString(string));

    manager=new QNetworkAccessManager(this);
    auto r=manager->get(QNetworkRequest(QUrl(url)));
    connect(r,&QNetworkReply::finished,[this,r]() {
        this->deReply(r);

    });

}

int map_model::rowCount(const QModelIndex &parent) const {
    jsonArray.size();
}

QVariant map_model::data(const QModelIndex &index, int role) const {
    if(index.row()>=jsonArray.size()){
        return QVariant();
    }
    QVariant var;
    switch (role){
        case ELModelInterface::ModelRoles::Role_MIME_TYPE:{
            var=MAP_TYPE;
            break;
        }
        case ELModelInterface::Role_MAP_POIID:{
            var=jsonArray.at(index.row())["id"];
            break;
        }
        case ELModelInterface::Role_TITLE:{
            var=jsonArray.at(index.row())["name"];
            break;
        }
        case  ELModelInterface::Role_ICON_ByteArray:{
            //TODO default icon
            var=mapIcon.value(index.row());
        }
        default:break;
    }
    return var;

}

void map_model::deReply(QNetworkReply *rpl) {
    QTextStream qts(rpl);
    auto obj=QJsonDocument::fromJson(qts.readAll().toLocal8Bit()).object();
    if(obj["info"]!="OK") {
        return ;
    }
    jsonArray=obj["pois"].toArray();

    for(auto i=0;i!=jsonArray.size();++i){
        auto item=jsonArray.at(i);
        auto ar=item.toObject()["photos"].toArray();
        if(!ar.empty()){
            qDebug()<<"url:"<<ar.at(0)["url"].toString();
            auto r=manager->get(QNetworkRequest(QUrl(ar.at(0)["url"].toString())));
            connect(r,&QNetworkReply::finished,[this,r,i]() {
                imageReady(r,i);
            });
        }

    }
    rpl->deleteLater();

}

void map_model::imageReady(QNetworkReply *rpl, int i) {
    auto all=rpl->readAll();
    qDebug()<<"data:"<<all;
    mapIcon.insert(i,all);
    // INFO some don't have img
//    if(i==jsonArray.size()){
//        emit resultsReady();
//    }
    emit resultsReady();
    rpl->deleteLater();

}

map_model::map_model() {
    this->setObjectName("MapModel");
    this->setDisplayName(tr("地图"));
}

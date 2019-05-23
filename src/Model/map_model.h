//
// Created by tender on 19-5-23.
//

#ifndef EVERYLAUNCHER_MAP_MODEL_H
#define EVERYLAUNCHER_MAP_MODEL_H


#include "ELModelInterface.h"
#include <QNetworkReply>
#include <QJsonArray>

class map_model: public ELModelInterface{
    Q_OBJECT
public:
    map_model();

    void search(std::string &string) override;

    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

private slots:
    void deReply(QNetworkReply *rpl);
    void imageReady(QNetworkReply* rpl,int i);

private:
    QNetworkAccessManager *manager{nullptr};
    QMap<int,QByteArray> mapIcon;
    QJsonArray jsonArray;
    const QString restUrl{"https://restapi.amap.com/v3/place/around?key=316c802a841aeb2f22d85c78f8ae2ef3&"
                    "location=%1&keywords=%2&types=&radius=1000&offset=3&page=1&extensions=all"};

};


#endif //EVERYLAUNCHER_MAP_MODEL_H

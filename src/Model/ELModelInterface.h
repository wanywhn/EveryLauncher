//
// Created by tender on 19-5-22.
//

#ifndef EVERYLAUNCHER_ELMODELINTERFACE_H
#define EVERYLAUNCHER_ELMODELINTERFACE_H

#include <string>
#include <QAbstractListModel>
#include "Model/ELModelInterface.h"

static const QString MAP_TYPE{"MAP_TYPE"};
class ELModelInterface: public QAbstractListModel {
    Q_OBJECT
public:
    enum ModelRoles{
        Role_FILE_NAME = Qt::UserRole + 1,
        Role_LOCATION = Qt::UserRole + 2,
        Role_ICON_ByteArray=Qt::UserRole+3,
        Role_FILE_SIMPLE_CONTENT = Qt::UserRole + 4,
        Role_MIME_TYPE = Qt::UserRole + 5,
        Role_RELEVANCY=Qt::UserRole+6,
        Role_APP_COMMENT=Qt::UserRole+7,
        Role_APP_NAME=Qt::UserRole+8,
        Role_NODISPLAY=Qt::UserRole+10,
        Role_APP_VERSION=Qt::UserRole+11,
        Role_TITLE=Qt::UserRole+12,
        Role_FILE_FULLTEXT_COLORED_FROM_RAW=Qt::UserRole+13,
        Role_FILE_FULLTEXT_COLORED_FROM_CACHED=Qt::UserRole+14,
        Role_MAP_POIID=Qt::UserRole+15,
    };



public:
    ELModelInterface(QObject *parent=nullptr) : QAbstractListModel(parent) {}

    virtual void search(std::string &){}
    signals:
    void resultsReady();

};



#endif //EVERYLAUNCHER_ELMODELINTERFACE_H

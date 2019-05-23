//
// Created by tender on 19-5-22.
//

#ifndef EVERYLAUNCHER_ELMODELINTERFACE_H
#define EVERYLAUNCHER_ELMODELINTERFACE_H

#include <string>
#include <QAbstractListModel>

class ELModelInterface {
public:
    enum ModelRoles{
        Role_FILE_NAME = Qt::UserRole + 1,
        Role_LOCATION = Qt::UserRole + 2,
        Role_ICON_PATH=Qt::UserRole+3,
        Role_FILE_SIMPLE_CONTENT = Qt::UserRole + 4,
        Role_FILE_FULLTEXT_COLORED_FROM_RAW=Qt::UserRole+13,
        Role_FILE_FULLTEXT_COLORED_FROM_CACHED=Qt::UserRole+14,
        Role_MIME_TYPE = Qt::UserRole + 5,
        Role_RELEVANCY=Qt::UserRole+6,
        Role_APP_COMMENT=Qt::UserRole+7,
        Role_APP_NAME=Qt::UserRole+8,
        Role_NODISPLAY=Qt::UserRole+10,
        Role_APP_VERSION=Qt::UserRole+11,
        Role_TITLE=Qt::UserRole+12,
    };
public :
    virtual void search(std::string &)=0;
};

#endif //EVERYLAUNCHER_ELMODELINTERFACE_H

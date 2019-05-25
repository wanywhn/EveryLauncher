//
// Created by tender on 19-5-22.
//

#ifndef EVERYLAUNCHER_ELMODELINTERFACE_H
#define EVERYLAUNCHER_ELMODELINTERFACE_H

#include <string>
#include <QAbstractListModel>
#include "Model/ELModelInterface.h"
#include <QDebug>
#include <QSettings>

static const QString MAP_TYPE{"MAP_TYPE"};
static const QString WEATHER_TYPE{"WEATHER_TYPE"};

class ELModelInterface : public QAbstractListModel {
Q_OBJECT
public:
    enum ModelRoles {
        Role_FILE_NAME = Qt::UserRole + 1,
        Role_LOCATION = Qt::UserRole + 2,
        Role_ICON_ByteArray = Qt::UserRole + 3,
        Role_FILE_SIMPLE_CONTENT = Qt::UserRole + 4,
        Role_MIME_TYPE = Qt::UserRole + 5,
        Role_RELEVANCY = Qt::UserRole + 6,
        Role_APP_COMMENT = Qt::UserRole + 7,
        Role_APP_NAME = Qt::UserRole + 8,
        Role_NODISPLAY = Qt::UserRole + 10,
        Role_APP_VERSION = Qt::UserRole + 11,
        //used to display in list
                Role_TITLE = Qt::UserRole + 12,
        Role_FILE_FULLTEXT_COLORED_FROM_RAW = Qt::UserRole + 13,
        Role_FILE_FULLTEXT_COLORED_FROM_CACHED = Qt::UserRole + 14,
        Role_MAP_POIID = Qt::UserRole + 15,
    };

public:
    /*
     * reload data when source changed
     */
    virtual void sourceChanged() {}

    /*
     * clear model data
     */
    virtual void clearSource()=0;

public:
    ELModelInterface(QObject *parent = nullptr) : QAbstractListModel(parent) {}

    virtual void search(QString &) = 0;

    bool operator<(const ELModelInterface &rhs) const {
        return this->displayPriority() < rhs.displayPriority();
    }

    /*
     * wheather this model is enabled.it can be disabled by user,default is enable
     */
    virtual bool isEnable() {
        QSettings settings;
        settings.beginGroup("Model");
        settings.beginGroup("Enabled");
        return settings.value(this->objectName(), true).toBool();
    }

    virtual void initDisplayPriority(int priority) {
        QSettings settings;
        settings.beginGroup("Model");
        settings.beginGroup("Priority");
        bool i{false};
        auto v = settings.value(this->objectName()).toInt(&i);
        if (i) {
            qDebug() << "to int false";
            return;
        }
        settings.setValue(this->objectName(), priority);
    }

protected:
    virtual int displayPriority() const {
        QSettings settings;
        settings.beginGroup("Model");
        settings.beginGroup("Priority");
        return settings.value(this->objectName(), 1).toInt();
    }

    virtual void setDisplayName(QString name) {
        QSettings settings;
        settings.beginGroup("Model");
        settings.beginGroup("Display");
        settings.setValue(this->objectName(), name);

    }

signals:

    /*
     * the search has completed,can read data
     */
    void resultsReady();

};


#endif //EVERYLAUNCHER_ELMODELINTERFACE_H

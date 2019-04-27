#ifndef RECOLLQUERYMODEL_H
#define RECOLLQUERYMODEL_H

#include <QObject>
#include <QStandardItemModel>
class RecollQueryModel : public QStandardItemModel
{
    Q_OBJECT
    int Role_FILE_NAME = Qt.UserRole + 1
   Role_LOCATION = Qt.UserRole + 2
   Role_FILE_STATUS = Qt.UserRole + 3
   Role_FILE_SIMPLE_CONTENT = Qt.UserRole + 4
   Role_TYPE = Qt.UserRole + 5
   Role_FILE_ICON=Qt.UserRole+6
   Role_FILE_EXEC=Qt.UserRole+7
   Role_DESKTOP_NAME=Qt.UserRole+8
   Role_DESKTOP_DESC=Qt.UserRole+9
public:
    RecollQueryModel();

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
};

#endif // RECOLLQUERYMODEL_H

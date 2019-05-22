#ifndef UNITEDMODEL_H
#define UNITEDMODEL_H

#include <QAbstractItemModel>
#include <docseq.h>
#include <QtCore/QSortFilterProxyModel>
#include "recollmodel.h"

class UnitedModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit UnitedModel(QObject *parent = nullptr);

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent ) const override;


    QVariant data(const QModelIndex &index, int role) const override;

public slots:
    void startSearch(std::string str);

    signals:

private:
    QVector<RecollModel *> rmodel;
    QSortFilterProxyModel *filterNone;
    QVector<QAbstractListModel *> lmodel;
};

#endif // UNITEDMODEL_H

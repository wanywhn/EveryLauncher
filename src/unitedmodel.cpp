#include "unitedmodel.h"

UnitedModel::UnitedModel(QObject *parent)
    : QAbstractListModel(parent)
{
}


QModelIndex UnitedModel::index(int row, int column, const QModelIndex &parent) const
{
    // FIXME: Implement me!
}


int UnitedModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}


QVariant UnitedModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}

int UnitedModel::columnCount(const QModelIndex &parent) const {

}

QModelIndex UnitedModel::parent(const QModelIndex &index) const {

}

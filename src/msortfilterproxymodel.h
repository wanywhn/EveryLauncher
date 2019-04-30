#ifndef MSORTFILTERPROXYMODEL_H
#define MSORTFILTERPROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>

class MSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    MSortFilterProxyModel(QObject *parent);

    // QAbstractItemModel interface
signals:
    void itemCountChanged(int count) const;
public:

    // QSortFilterProxyModel interface
    int getMaxItemCount() const;
    void setMaxItemCount(int value);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
private:
    int currentItemCount;
    int maxItemCount;

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override;

    // QAbstractProxyModel interface
public:
//    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
//    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;

    // QAbstractItemModel interface
public:
    QVariant data(const QModelIndex &index, int role) const override;

    // QAbstractItemModel interface
public:
    bool removeRows(int row, int count, const QModelIndex &parent) override;
};

#endif // MSORTFILTERPROXYMODEL_H

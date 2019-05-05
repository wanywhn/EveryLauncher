#ifndef MSORTFILTERPROXYMODEL_H
#define MSORTFILTERPROXYMODEL_H

#include <QObject>
#include <QSet>
#include <QMap>
#include <QSortFilterProxyModel>

class MSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    MSortFilterProxyModel(QObject *parent);

    // QAbstractItemModel interface
public:

    // QSortFilterProxyModel interface
    int getMaxItemCount() const;
    void setMaxItemCount(int value);
    public slots:
        void resetPar();

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
private:
    int currentItemCount{0};
    int currentGroupCount{0};
    QString prevGroup;
    int maxItemCount{2};
    //proxy to source
    QMap<int,int> mapidx;
    QSet<int> setDot;
    QMap<int,QString> mapSections;
    bool firstLine{true};
    bool ommitTill{false};

public:
    int rowCount(const QModelIndex &parent) const override;
public:
    QVariant data(const QModelIndex &index, int role) const override;


    // QAbstractProxyModel interface
public:
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;

    // QAbstractItemModel interface
public:
    Qt::ItemFlags flags(const QModelIndex &index) const override;
};

#endif // MSORTFILTERPROXYMODEL_H

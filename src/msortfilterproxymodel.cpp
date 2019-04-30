#include "msortfilterproxymodel.h"
#include <QDebug>

MSortFilterProxyModel::MSortFilterProxyModel(QObject *parent):QSortFilterProxyModel (parent)
{
   currentItemCount=0;
   maxItemCount=15;
}


int MSortFilterProxyModel::getMaxItemCount() const
{
    return maxItemCount;
}

void MSortFilterProxyModel::setMaxItemCount(int value)
{
    maxItemCount = value;
}

bool MSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if(currentItemCount>=maxItemCount){
        return false;
    }
    auto accepted=QSortFilterProxyModel::filterAcceptsRow(source_row,source_parent);
    if(accepted){
        auto t=const_cast<MSortFilterProxyModel *>(this);
        t->currentItemCount++;
        emit itemCountChanged(t->currentItemCount);
    }
    return accepted;

}

int MSortFilterProxyModel::rowCount(const QModelIndex &parent) const
{
    auto cnt=QSortFilterProxyModel::rowCount(parent);
    return currentItemCount>0?cnt+1:cnt;

}

/*
QModelIndex MSortFilterProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if(proxyIndex.row()==0) {
        //dumpy item
        return QModelIndex();
    }else{
       return QSortFilterProxyModel::mapToSource(proxyIndex) ;
    }
}

QModelIndex MSortFilterProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    auto idx=sourceIndex.model()->index(sourceIndex.row()+1,sourceIndex.column());
    return QSortFilterProxyModel::mapFromSource(idx);

}
*/

QVariant MSortFilterProxyModel::data(const QModelIndex &index, int role) const
{
    if(index.row()==0){
        //dumpy item
        return "";
    }
    auto i=index.model()->index(index.row()-1,index.column());
    return QSortFilterProxyModel::data(i,role);

}

bool MSortFilterProxyModel::removeRows(int row, int count, const QModelIndex &parent)
{
    auto tober=qMin(currentItemCount,count);
    bool r=QSortFilterProxyModel::removeRows(row,tober,parent);
        qDebug()<<"remove:curr "<<currentItemCount;
    if(r){
        currentItemCount-=tober;
        qDebug()<<"remove:curr "<<currentItemCount;
    }
    return r;

}

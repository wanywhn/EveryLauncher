#include "msortfilterproxymodel.h"
#include "recollmodel.h"
#include <QDebug>

MSortFilterProxyModel::MSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent) {
  connect(this, &MSortFilterProxyModel::modelReset, this,
          &MSortFilterProxyModel::resetPar);
}

int MSortFilterProxyModel::getMaxItemCount() const { return maxItemCount; }

void MSortFilterProxyModel::setMaxItemCount(int value) {
  maxItemCount = value;
  invalidateFilter();
}

void MSortFilterProxyModel::resetPar() {
  currentGroupCount = 0;
  currentItemCount = 0;
  prevGroup="";
  maxItemCount=4;
  mapidx.clear();
  setDot.clear();
  mapSections.clear();
  ommitTill=false;

}

bool MSortFilterProxyModel::filterAcceptsRow(
    int source_row, const QModelIndex &source_parent) const {
  auto t = const_cast<MSortFilterProxyModel *>(this);
  auto sourceIndex = sourceModel()->index(source_row, 0, source_parent);
  auto lineGroup = sourceIndex
                       .data(RecollModel::ModelRoles::Role_MIME_TYPE)
                       .toString();

  if(sourceIndex.data(RecollModel::ModelRoles::Role_NODISPLAY).toString().trimmed()=="true"){
      return false;
  }
  // new group ,add section
  if (prevGroup != lineGroup) {
    if (t->ommitTill) {
      // add  previous dot
      t->ommitTill = false;
      t->setDot.insert(currentItemCount);
      t->mapSections.insert(currentItemCount, prevGroup);
      t->currentItemCount++;
    }

    t->currentGroupCount = 0;
    // this is the section
    t->mapSections.insert(currentItemCount, lineGroup);
    t->currentItemCount++;
    t->prevGroup = lineGroup;

    // real item
    t->mapidx.insert(currentItemCount,source_row);
    t->currentItemCount++;
    t->currentGroupCount++;
    return true;
  }
  if (t->ommitTill) {
    return false;
  }
  // same group sa previous
  if (currentGroupCount > maxItemCount) {
    t->ommitTill = true;
    return false;
  }
  // add current
  t->mapidx.insert(currentItemCount, source_row);
  t->currentItemCount++;
  t->currentGroupCount++;
  return true;
}

int MSortFilterProxyModel::rowCount(const QModelIndex &parent) const {
  return currentItemCount;
}

QVariant MSortFilterProxyModel::data(const QModelIndex &index, int role) const {
  if (setDot.contains(index.row())) {
    if (role == RecollModel::ModelRoles::Role_VIEW_TYPE) {
      return "DOT";
    }
    if (role == RecollModel::ModelRoles::Role_MIME_TYPE) {
      return mapSections.value(index.row());
    }
  } else {
    if (mapidx.contains(index.row())) {
      if (role == RecollModel::ModelRoles::Role_VIEW_TYPE) {
        return "ITEM";
      }
      //      auto i = sourceModel()->index(mapidx.value(index.row()),
      //      index.column());
      return QSortFilterProxyModel::data(index, role);
    } else {
      if (role == RecollModel::ModelRoles::Role_VIEW_TYPE) {
        return "SECTION";
      }
      if (role == RecollModel::ModelRoles::Role_MIME_TYPE) {
        return mapSections.value(index.row());
      }
      // the section
    }
  }
  //	auto i=index.model()->index(index.row()-1,index.column());
  return QVariant();
  //  return QSortFilterProxyModel::data(index, role);
}

QModelIndex
MSortFilterProxyModel::mapToSource(const QModelIndex &proxyIndex) const {
  //  auto t = const_cast<MSortFilterProxyModel *>(this);
  if (mapidx.contains(proxyIndex.row())) {

    return sourceModel()->index(mapidx.value(proxyIndex.row()),
                                proxyIndex.column());
  }
  return QModelIndex();
}

Qt::ItemFlags MSortFilterProxyModel::flags(const QModelIndex &index) const {
  if (mapidx.contains(index.row())) {
    return QSortFilterProxyModel::flags(index);
  }
  return Qt::ItemFlag::ItemIsSelectable|Qt::ItemFlag::ItemIsEnabled;
}

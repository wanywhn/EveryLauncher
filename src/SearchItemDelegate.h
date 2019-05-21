//
// Created by tender on 19-5-21.
//

#ifndef EVERYLAUNCHER_SEARCHITEMDELEGATE_H
#define EVERYLAUNCHER_SEARCHITEMDELEGATE_H


#include "reslistwidget.h"

class SearchItemDelegate :public QStyledItemDelegate {

public:
    explicit SearchItemDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
};


#endif //EVERYLAUNCHER_SEARCHITEMDELEGATE_H

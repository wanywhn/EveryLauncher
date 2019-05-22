//
// Created by tender on 19-5-21.
//

#include "SearchItemDelegate.h"
#include "Model/recollmodel.h"
#include <QDebug>
#include <QPainter>

void SearchItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
//    auto mimeType =
//            index.data(RecollModel::Role_MIME_TYPE).toString();

    auto filename =
    index.data(RecollModel::Role_TITLE).toString();
            if(filename.isEmpty())
                filename=index.data(RecollModel::Role_FILE_NAME).toString();
//    if (index.data(RecollModel::Role_MIME_TYPE).toString() ==
//        "application/x-all") {
        // TODO find app icon
//        filename =
//                index.data(RecollModel::Role_APP_NAME).toString();
//    }
    auto iconpath =
            index.data(RecollModel::Role_ICON_PATH).toString();

    QPixmap icon(iconpath);
    if (icon.isNull()) {
//        qDebug() << "null icon:" << iconpath;
    }
    icon = icon.scaled(this->sizeHint(option, index));
    if (opt.state & QStyle::State_Selected) {
        painter->fillRect(opt.rect, opt.palette.highlight());
    }
    QRectF iconRectf(opt.rect);
    iconRectf.setSize(icon.size());

    painter->drawPixmap(iconRectf, icon, icon.rect());

    auto textPos = QPointF(iconRectf.topRight());

    textPos.ry() += iconRectf.height() / 2;
    painter->drawText(textPos, filename);
}

QSize SearchItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {

    //TODO hidpi?
    return {50, 50};
}

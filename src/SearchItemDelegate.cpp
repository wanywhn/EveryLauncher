//
// Created by tender on 19-5-21.
//

#include "SearchItemDelegate.h"

void SearchItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    auto itemType =
            index.data(RecollModel::Role_VIEW_TYPE).toString();
    auto mimeType =
            index.data(RecollModel::Role_MIME_TYPE).toString();
    if (itemType == "SECTION") {
        painter->drawText(opt.rect.adjusted(-1, -1, -1, -1), mimeType);
        if (opt.state & QStyle::State_Selected) {
            painter->fillRect(opt.rect, opt.palette.highlight());
        }
        return;
    } else if (itemType == "DOT") {
        painter->drawText(opt.rect.center(), "...");
        if (opt.state & QStyle::State_Selected) {
            painter->fillRect(opt.rect, opt.palette.highlight());
        }
        return;
    }

//    } else if (itemType == "ITEM") {

    auto filename =
            index.data(RecollModel::Role_FILE_NAME).toString();
    if (index.data(RecollModel::Role_MIME_TYPE).toString() ==
        "application/x-all") {
        // TODO find app icon
        filename =
                index.data(RecollModel::Role_APP_NAME).toString();
    }
    auto iconpath =
            index.data(RecollModel::Role_ICON_PATH).toString();

    QPixmap icon(iconpath);
    if (icon.isNull()) {
        qDebug() << "null icon:" << iconpath;
    }
    icon = icon.scaled(this->sizeHint(option, index));
    QRectF recf(opt.rect);
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
    auto itemType =
            index.data(RecollModel::Role_VIEW_TYPE).toString();
    if (itemType == "SECTION") {


        return {200, 25};
    } else if (itemType == "DOT") {

        return {200, 25};
    }
    return {50, 50};
}

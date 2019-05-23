#include <docseqdb.h>
#include <QtWidgets/QProgressDialog>
#include <QProgressDialog>
#include <wasatorcl.h>
#include <QThread>
#include <QtWidgets/QMessageBox>
#include <DtkWidgets>
#include <QtCore/QStringListModel>
#include <Model/ModelWeather.h>
#include "unitedmodel.h"
#include "config.h"
#include "searchline.h"
#include "map_model.h"



UnitedModel::UnitedModel(QObject *parent)
        : QAbstractListModel(parent) {

    QMap<QString, std::string> k2k{
            {"app",          "app"},
            {"text",         "text"},
            {"spreadsheet",  "spreadsheet"},
            {"presentation", "presentation"},
            {"media",        "media"},
            {"message",      "message"},
            {"other",        "other"},
    };
//    for(const auto &item:k2k.keys()){
//        string frag;
//        theconfig->getGuiFilter(k2k.value(item), frag);
//        DocSeqFiltSpec m_filtspec;
//        m_filtspec.orCrit(DocSeqFiltSpec::DSFS_QLANG, frag);
    auto m = new RecollModel;
//        m->setFilterSpec(m_filtspec);
    auto filterNone = new MFilterModel(m);
    lmodel.push_back(filterNone);

    // FIXME there
    lmodel.push_back(new map_model());
    for(auto item:lmodel){
    connect(item, &ELModelInterface::resultsReady, [this]() {
        qDebug() << "ready";
        this->beginResetModel();
        this->endResetModel();
    });
    }

//        m_model=m;
//    }
}


QModelIndex UnitedModel::index(int row, int column, const QModelIndex &parent) const {
    return createIndex(row, column, row);
}


int UnitedModel::rowCount(const QModelIndex &parent) const {
//    if (!parent.isValid())
//        return 0;
    auto totoalcnt = 0;
    for (auto item:lmodel) {
        if (item->rowCount() > 0) {
            totoalcnt += item->rowCount();//+1;
        }
    }
//return filterNone->rowCount(parent);
    auto ncthis = const_cast<UnitedModel *>(this);
    ncthis->rowNumber = totoalcnt;
    qDebug()<<"rowcnt:"<<totoalcnt;
    return totoalcnt;
}


QVariant UnitedModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    //TODO map
    if (index.row() >= rowNumber) {
        return QVariant();
    }
    int r = index.row();
    auto fitem = lmodel.first();
    for (auto item:lmodel) {
        if (r >= item->rowCount()) {
            r -= item->rowCount();
            continue;
        }
        fitem = item;
        break;

    }
    return fitem->data(fitem->index(r, index.column(), index.parent()), role);
}

QModelIndex UnitedModel::parent(const QModelIndex &index) const {

    return {};
}

void UnitedModel::startSearch(std::string str) {
    //TODO search in every model

    for (auto item:lmodel) {
            item->search(str);
    }
}

void UnitedModel::cleanSearch() {
    //TODO clean res
    for(auto item:lmodel){
    }

}

extern bool maybeOpenDb(string &reason, bool force, bool *maindberror);



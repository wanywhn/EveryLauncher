#include <docseqdb.h>
#include <QtWidgets/QProgressDialog>
#include <QProgressDialog>
#include <wasatorcl.h>
#include <QThread>
#include <QtWidgets/QMessageBox>
#include <DtkWidgets>
#include <QtCore/QStringListModel>
#include <Model/ModelWeather.h>
#include <systemtray.h>
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
//        m->setFilterSpec(m_filtspec);

    auto m = new RecollModel;
    auto filterNone = new MFilterModel(m);
    lmodel.push_back(filterNone);
    lmodel.push_back(new map_model());
    reloadModel();
    connect(&SystemTray::getInstance(nullptr),&SystemTray::ConfigChanged,this,&UnitedModel::reloadModel);

//        m_model=m;
//    }
}

void UnitedModel::reloadModel() {
    for(auto item:usermodel){
        item->disconnect();
//        this->disconnect(item);
    }
    usermodel.clear();
    for (auto i = 0; i != lmodel.size(); ++i) {
        auto item = lmodel.at(i);
        item->initDisplayPriority(i);
        if (item->isEnable()) {
            usermodel.push_back(item);
        }
    }
    //TODO re read when user change order or enabled
    std::sort(usermodel.begin(), usermodel.end());
    qDebug() << usermodel;
    for (auto item:usermodel) {
        connect(item, &ELModelInterface::resultsReady, [this,item]() {
            qDebug() <<item<< "ready";
            beginResetModel();
            endResetModel();
        });
    }
}


QModelIndex UnitedModel::index(int row, int column, const QModelIndex &parent) const {
    return createIndex(row, column, row);
}


int UnitedModel::rowCount(const QModelIndex &parent) const {
//    if (!parent.isValid())
//        return 0;
    auto totoalcnt = 0;
    for (auto item:usermodel) {
        if (item->rowCount() > 0) {
            totoalcnt += item->rowCount();//+1;
        }
    }
//return filterNone->rowCount(parent);
    auto ncthis = const_cast<UnitedModel *>(this);
    ncthis->rowNumber = totoalcnt;
    qDebug() << "rowcnt:" << totoalcnt;
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
    auto fitem = usermodel.first();
    for (auto item:usermodel) {
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

void UnitedModel::startSearch(QString str) {
    //TODO search in every model
    // get k1&k2
    // if not: normal search ,get x result in model in user defined order,
    // else use k1&k2 for specific search
    // emit search hint

    QString k1, k2;
    getOption(str, k1, k2);
    if (k1.isEmpty() || k2.isEmpty()) {


    }

    /*
    QString s("");
    for (auto tmp : str) {
        if ((tmp >= 'a' && tmp <= 'z') || (tmp >= 'A' && tmp <= 'Z')) {
            s += tmp + QString("*");
        } else {
            s += tmp;
        }
    }
     */


    for (auto item:usermodel) {
        auto s = str.toStdString();
        item->search(s);
    }
}

void UnitedModel::cleanSearch() {
    //TODO clean res
    for (auto item:usermodel) {
    }

}

void UnitedModel::getOption(QString &basic_string, QString &qString, QString &k2) {
    auto sped = basic_string.split(" ");
    Q_ASSERT(!sped.empty());
    auto k1k2 = sped.at(0).split(":");
    if (k1k2.size() != 2) {
        return;
    }
    qString = k1k2.at(0);
    k2 = k1k2.at(1);


}

extern bool maybeOpenDb(string &reason, bool force, bool *maindberror);



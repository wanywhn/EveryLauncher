#include <docseqdb.h>
#include <QtWidgets/QProgressDialog>
#include <QProgressDialog>
#include <wasatorcl.h>
#include <QThread>
#include <QtWidgets/QMessageBox>
#include <DtkWidgets>
#include "unitedmodel.h"
#include "config.h"
#include "searchline.h"

UnitedModel::UnitedModel(QObject *parent)
        : QAbstractListModel(parent) {

    QMap<QString,std::string> k2k{
            {"app","app"},
            {"text","text"},
            {"spreadsheet","spreadsheet"},
            {"presentation","presentation"},
            {"media","media"},
            {"message","message"},
            {"other","other"},
    };
//    for(const auto &item:k2k.keys()){
//        string frag;
//        theconfig->getGuiFilter(k2k.value(item), frag);
//        DocSeqFiltSpec m_filtspec;
//        m_filtspec.orCrit(DocSeqFiltSpec::DSFS_QLANG, frag);
        auto m=new RecollModel;
        connect(m,&RecollModel::restultReady,[this](){
            qDebug()<<"ready";
            this->beginResetModel();
            this->endResetModel();

        });
//        m->setFilterSpec(m_filtspec);
filterNone=new QSortFilterProxyModel;
    filterNone->setFilterRole(RecollModel::ModelRoles::Role_NODISPLAY);
    filterNone->setFilterRegExp("^((?!true).)*$");
    filterNone->setSourceModel(m);
        rmodel.push_back(m);
//    }
}


QModelIndex UnitedModel::index(int row, int column, const QModelIndex &parent) const {
    return filterNone->index(row,column,parent);
    // FIXME: Implement me!
}


int UnitedModel::rowCount(const QModelIndex &parent) const {
//    if (!parent.isValid())
//        return 0;
    auto totoalcnt=0;
//    for(auto item:rmodel){
//        if(item->rowCount(QModelIndex())>0){
//            totoalcnt+=item->rowCount(QModelIndex());//+1;
//        }
//    }
//    for(auto item:lmodel){
//        if(item->rowCount(QModelIndex())>0){
//            totoalcnt+=item->rowCount(QModelIndex())+1;
//        }
//    }
return filterNone->rowCount(parent);
    return totoalcnt;
}


QVariant UnitedModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    return filterNone->data(index,role);
    // FIXME: Implement me!
    return QVariant();
}

QModelIndex UnitedModel::parent(const QModelIndex &index) const {

}

void UnitedModel::startSearch(std::string str) {
    //TODO search in every model
    for(auto item:rmodel){
        item->search(str);
    }
}

extern bool maybeOpenDb(string &reason, bool force, bool *maindberror);



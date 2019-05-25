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
#include <widget.h>
#include "unitedmodel.h"
#include "config.h"
#include "searchline.h"
#include "map_model.h"

// FIXME this is a workaround
class MFilterModel : public ELModelInterface {
Q_OBJECT
public:
    explicit MFilterModel(RecollModel *parent) : ELModelInterface(parent), model(parent) {
        sfmodel = new QSortFilterProxyModel();
        sfmodel->setSourceModel(model);
        sfmodel->setFilterRole(RecollModel::ModelRoles::Role_NODISPLAY);
        sfmodel->setFilterRegExp("^((?!true).)*$");
        sfmodel->setDynamicSortFilter(true);
        connect(model, &RecollModel::resultsReady, this, &ELModelInterface::resultsReady);
    }

    void setSpec(const QString &objName, const QString &dspName) {
//        this->setObjectName("MFilterModel");
//        this->setDisplayName(tr("×文档"));
        this->setObjectName(objName);
        this->setDisplayName(dspName);
    }

    int rowCount(const QModelIndex &parent) const override {
        return sfmodel->rowCount();
    }

    QVariant data(const QModelIndex &index, int role) const override {
        return sfmodel->data(index, role);
    }

    QModelIndex index(int row, int column, const QModelIndex &parent) const override {
        return sfmodel->index(row, column, parent);
    }


    void search(QString &string1) override {
        model->search(string1);
    }

    void clearSource() override {
        model->clearSource();
    }

    void sourceChanged() override {
        model->sourceChanged();
    }


private:
    RecollModel *model;
    QSortFilterProxyModel *sfmodel;

};

#include "unitedmodel.moc"

static QVector<QStringList> modelarg = {
        {"AppModel",          QObject::tr("程序"),  "app"},
        {"TextModel",         QObject::tr("文本"),  "text"},
        {"SpreadSheetModel",  QObject::tr("表格"),  "spreadsheet"},
        {"PresentationModel", QObject::tr("PPT"), "presentation"},
        {"MediaModel",        QObject::tr("媒体"),  "media"},
        {"MessageModel",      QObject::tr("信息"),  "message"},
        {"OtherModel",        QObject::tr("其他"),  "other"},
};

UnitedModel::UnitedModel(MainWindow *parent)
        : QAbstractListModel(parent) {

    indexModule = new IndexModule;

    RecollModel *m;
    MFilterModel *filterNone;
    for (const auto &item:modelarg) {
        m = new RecollModel(item.at(2));
        filterNone = new MFilterModel(m);
        filterNone->setSpec(item.at(0), item.at(1));
        lmodel.push_back(filterNone);
        connect(indexModule, &IndexModule::sourceChanged, filterNone, &ELModelInterface::sourceChanged);
    }
    lmodel.push_back(new map_model());
    reloadModel();
    connect(&SystemTray::getInstance(parent), &SystemTray::ConfigChanged, this, &UnitedModel::reloadModel);

}


void UnitedModel::reloadModel() {
    //TODO re read when user change order or enabled
    //!!TODO optim

    for (auto item:usermodel) {
        item->disconnect();
    }
    usermodel.clear();
    for (auto i = 0; i != lmodel.size(); ++i) {
        auto item = lmodel.at(i);
        item->initDisplayPriority(i);

        if (item->isEnable()) {
            usermodel.push_back(item);
        }
    }
    std::sort(usermodel.begin(), usermodel.end());
    qDebug() << usermodel;
    for (auto item:usermodel) {
        connect(item, &ELModelInterface::resultsReady, [this, item]() {
            qDebug() << item << "ready";
            beginResetModel();
            endResetModel();
        });
    }
}


QModelIndex UnitedModel::index(int row, int column, const QModelIndex &parent) const {
    return createIndex(row, column, row);
}


int UnitedModel::rowCount(const QModelIndex &parent) const {
    auto totoalcnt = 0;
    for (auto item:usermodel) {
        if (item->rowCount() > 0) {
            totoalcnt += item->rowCount();//+1;
        }
    }
    auto ncthis = const_cast<UnitedModel *>(this);
    ncthis->rowNumber = totoalcnt;
    qDebug() << "rowcnt:" << totoalcnt;
    return totoalcnt;
}


QVariant UnitedModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    //TODO section
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

    for (auto item:usermodel) {
//        auto str = str.toStdString();
        if (item->inherits("MFilterModel")) {
            QString s("");
            for (auto tmp : str) {
                if ((tmp >= 'a' && tmp <= 'z') || (tmp >= 'A' && tmp <= 'Z')) {
                    s += tmp + QString("*");
                } else {
                    s += tmp;
                }
            }
            qDebug() << "MFileterModel";
            item->search(s);
        } else {
            item->search(str);
        }
    }
}

void UnitedModel::cleanSearch() {
    //TODO clean res
    for (auto item:usermodel) {
        item->clearSource();
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



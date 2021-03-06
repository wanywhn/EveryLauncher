//
// Created by tender on 19-5-24.
//

#include "ConfigModelEngin.h"
#include <QHeaderView>
#include <QSettings>
#include <QVBoxLayout>
#include <queue>
#include <QMap>
#include <QAbstractTableModel>

//TODO restore when user cancel?
class ConfigModelEngineModel : public QAbstractTableModel {
public:
    explicit ConfigModelEngineModel(QObject *parent = nullptr) : QAbstractTableModel(parent) {
        QSettings sett;
        sl << "来源"
           << "勾选启用";
        //TODO 在unitedmodel中删除某个模型时，这里还会显示。貌似是结构性的问题。


        sett.beginGroup("Model");
        sett.beginGroup("Priority");

        vs.reserve(sett.childKeys().size());
        for (const auto &item:sett.childKeys()) {
            vs.push_back({sett.value(item).toInt(), item});
        }
        std::sort(vs.begin(), vs.end(), [](const QPair<int, QString> &lhs, const QPair<int, QString> &rhs) {
            return lhs.first < rhs.first;

        });

        sett.endGroup();
        sett.beginGroup("Enabled");

        for (const auto &item:sett.childKeys()) {
            if (!sett.value(item).toBool())
                ve.insert(item);
        }
        sett.endGroup();
        sett.beginGroup("Display");
        for (auto item:sett.childKeys()) {
            vd.insert(item, sett.value(item, "None").toString());
        }

        Q_ASSERT(vd.size() == vs.size());

        beginResetModel();
        endResetModel();

        sett.endGroup();
        sett.endGroup();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
        if (orientation == Qt::Orientation::Vertical)
            return QAbstractItemModel::headerData(section, orientation, role);
        return sl.at(section);
    }

    Qt::ItemFlags flags(const QModelIndex &index) const override {
        return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable;
    }

    QModelIndex parent(const QModelIndex &child) const override {
        return {};
    }

    int rowCount(const QModelIndex &parent) const override {
        return vs.size();

    }

    int columnCount(const QModelIndex &parent) const override {
        return 1;
    }

    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid() || index.row() >= vs.size())
            return QVariant();
        if (role == Qt::DisplayRole) {
            return vd.value(vs.at(index.row()).second);
        }
        if (role == Qt::CheckStateRole) {
            auto disabled = ve.contains(vs.at(index.row()).second);
            if (disabled) {
                return Qt::Unchecked;
            } else {
                return Qt::Checked;
            }
        }
        if(role==Qt::DecorationRole)
            return QIcon(":/icon/default.svg");
        return {};
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role) override {
        if (role == Qt::CheckStateRole) {
            auto checked = value.toBool();
            QSettings sett;
            sett.beginGroup("Model");
            sett.beginGroup("Enabled");
            auto k = vs.at(index.row()).second;
            if (checked) {
//                sett.remove(k);
                sett.setValue(k,true);
                ve.remove(k);
            } else {
                sett.setValue(k, false);
                ve.insert(k);
            }
            emit dataChanged(index,index);
        }
        return true;
    }

private:
    QSet<QString> ve;
    QMap<QString, QString> vd;
    QVector<QPair<int, QString>> vs;
    QStringList sl;


};

ConfigModelEngine::ConfigModelEngine(QWidget *parent) : QWidget(parent) {
    init_ui();
}


void ConfigModelEngine::init_ui() {
    auto mlayout = new QVBoxLayout();
    this->setLayout(mlayout);
    twidget = new QTableView();
    twidget->setModel(new ConfigModelEngineModel);


    twidget->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    twidget->horizontalHeader()->setStretchLastSection(true);
    twidget->verticalHeader()->setHidden(true);

    mlayout->addWidget(twidget);

}


#ifndef UNITEDMODEL_H
#define UNITEDMODEL_H

#include <QAbstractItemModel>
#include <docseq.h>
#include <QtCore/QSortFilterProxyModel>
#include <queue>
#include "Model/recollmodel.h"

// FIXME this is a workaround
class MFilterModel :public ELModelInterface {
    Q_OBJECT
public:
    explicit MFilterModel(RecollModel *parent) :ELModelInterface(parent), model(parent) {
        this->setObjectName("MFilterModel");
        this->setDisplayName(tr("×文档"));
        sfmodel=new QSortFilterProxyModel();
        sfmodel->setSourceModel(model);
        sfmodel->setFilterRole(RecollModel::ModelRoles::Role_NODISPLAY);
        sfmodel->setFilterRegExp("^((?!true).)*$");
        sfmodel->setDynamicSortFilter(true);
        connect(model,&RecollModel::resultsReady,this,&ELModelInterface::resultsReady);
    }

    int rowCount(const QModelIndex &parent) const override {
        return sfmodel->rowCount();
    }

    QVariant data(const QModelIndex &index, int role) const override {
        return sfmodel->data(index,role);
    }

    QModelIndex index(int row, int column, const QModelIndex &parent) const override {
        return sfmodel->index(row,column,parent);
    }


    void search(std::string &string1) override {
        model->search(string1);

    }


private:
    RecollModel *model;
    QSortFilterProxyModel *sfmodel;

};
class UnitedModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit UnitedModel(QObject *parent = nullptr);

    QModelIndex index(int row, int column,
                      const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent ) const override;


    QVariant data(const QModelIndex &index, int role) const override;

public slots:
    void startSearch(QString str);
    void cleanSearch();


private:
    int rowNumber{0};
    QVector<ELModelInterface *>usermodel;
    QVector<ELModelInterface *> lmodel;

    void getOption(QString &basic_string, QString &qString, QString &k2);

    void reloadModel();
};

#endif // UNITEDMODEL_H

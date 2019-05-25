#ifndef UNITEDMODEL_H
#define UNITEDMODEL_H

#include <QAbstractItemModel>
#include <docseq.h>
#include <QtCore/QSortFilterProxyModel>
#include <queue>
#include <QtCore/QMutex>
#include <QtCore/QProcess>
#include <IndexModule.h>
#include <widget.h>
#include "widget.h"
#include "Model/recollmodel.h"

class MainWindow;

class UnitedModel : public QAbstractListModel {
    Q_OBJECT
public:
    explicit UnitedModel(MainWindow * parent = nullptr);

    QModelIndex index(int row, int column,
                      const QModelIndex &parent) const override;

    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent) const override;


    QVariant data(const QModelIndex &index, int role) const override;

public slots:

    void startSearch(QString str);

    void cleanSearch();


private:
    int rowNumber{0};
    QVector<ELModelInterface *> usermodel;
    QVector<ELModelInterface *> lmodel;
    IndexModule *indexModule;

    void getOption(QString &basic_string, QString &qString, QString &k2);

    void reloadModel();

};

#endif // UNITEDMODEL_H

#ifndef RESLISTWIDGET_H
#define RESLISTWIDGET_H

#include <QAbstractListModel>
#include <QLabel>
#include <QListView>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QWidget>
#include "detailedwidget.h"

#include "docseq.h"
#include "plaintorich.h"
#include "Model/recollmodel.h"
#include "Model/unitedmodel.h"

#include <DtkWidgets>
#include <dde-launcher/dbusfileinfo.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

using std::list;
using std::map;
using std::string;
using std::vector;


class QUrl;

DWIDGET_USE_NAMESPACE
class ResWidget : public QWidget{

  Q_OBJECT

public:
    explicit ResWidget(QWidget *parent = nullptr);

    ~ResWidget() override = default;

    virtual UnitedModel * getModel() { return m_model; }

private:
  void init_ui();
  void init_conn();
private slots:
  virtual void onTableView_currentChanged(QModelIndex &index);
public slots:

  void returnPressed();
  void currentMoveUp();
  void currentMoveDown();
  void moveToNextResoule();
  void useFilterProxy();

signals:
  void filterChanged(QString filed);
//  void currentChanged();
private:
  DListView *listview;
  UnitedModel *m_model;
public:
    void setM_model(UnitedModel *m_model);

private:
    bool m_ismainres;
    DetailedWidget *dtw;
    int mdetailRow{-1};
};

#endif // RESLISTWIDGET_H

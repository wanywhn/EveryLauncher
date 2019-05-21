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
#include "msortfilterproxymodel.h"
#include "plaintorich.h"
#include "recollmodel.h"

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
class ResTable : public QWidget{

  Q_OBJECT

public:
    explicit ResTable(QWidget *parent = nullptr);

    ~ResTable() override = default;

    virtual RecollModel *getModel() { return m_model; }

private:
  void init_ui();
  void init_conn();
private slots:
  virtual void onTableView_currentChanged();
public slots:
  virtual void setDocSource(std::shared_ptr<DocSequence> nsource);
  virtual void resetSource();
  virtual void readDocSource(bool resetPos = true);
  void clearSeach();
  void returnPressed();
  void currentMoveUp();
  void currentMoveDown();
  void moveToNextResoule();
  void useFilterProxy();

signals:
  void filterChanged(QString filed);
  void currentChanged();
private:
  DListView *listview;
  QSortFilterProxyModel *filterNone;
  MSortFilterProxyModel *proxyModel;
  QSortFilterProxyModel *currentFilterModel;
  RecollModel *m_model;
  bool m_ismainres;
    DetailedWidget *dtw;
    int mdetailRow{-1};
};

#endif // RESLISTWIDGET_H

#ifndef RESLISTWIDGET_H
#define RESLISTWIDGET_H

#include <QAbstractListModel>
#include <QLabel>
#include <QListView>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QWidget>
#include <detailedwidget.h>

#include "docseq.h"
#include "msortfilterproxymodel.h"
#include "plaintorich.h"
#include "recollmodel.h"

#include <DtkWidgets>
#include <map>
#include <memory>
#include <string>
#include <vector>

using std::list;
using std::map;
using std::string;
using std::vector;

extern RclConfig *theconfig;
// class ResTable;


// class ResTablePager;
class QUrl;
// class RclMain;

DWIDGET_USE_NAMESPACE
class ResTable : public QWidget{

  Q_OBJECT

public:
  ResTable(QWidget *parent = nullptr);

  virtual ~ResTable() {}
  virtual RecollModel *getModel() { return m_model; }
  //    virtual ResTableDetailArea* getDetailArea() {return m_detail;}
  virtual int getDetailDocNumOrTopRow();

  //    void setRclMain(RclMain *m, bool ismain);

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
  virtual void makeRowVisible(int row);
  virtual void takeFocus();
  void moveToNextResoule();
  void useFilterProxy();

signals:
  void filterChanged(QString filed);
  //    friend class ResTablePager;
  //    friend class ResTableDetailArea;
  void currentChanged();
private:
//  QVector<QPair<DListView *, MSortFilterProxyModel *> > vm;
  DListView *listview;
  QSortFilterProxyModel *filterNone;
  MSortFilterProxyModel *proxyModel;
  //!! only store listview etc
  QVBoxLayout *llayout;
  RecollModel *m_model;
  int mdetailRow;
  Rcl::Doc m_detaildoc;
  int m_popcolumn;
  bool m_ismainres;
    //TODO multi
    DetailedWidget *dtw;
};

#endif // RESLISTWIDGET_H

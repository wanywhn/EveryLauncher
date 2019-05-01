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
  void init();
  virtual void onTableView_currentChanged();
public slots:
  virtual void setDocSource(std::shared_ptr<DocSequence> nsource);
  virtual void resetSource();
  virtual void readDocSource(bool resetPos = true);
  void clearSeach();
//  virtual void onSortDataChanged(DocSeqSortSpec);
  virtual void makeRowVisible(int row);
  virtual void takeFocus();
  void moveToNextResoule();

  //    friend class ResTablePager;
  //    friend class ResTableDetailArea;
private:
  QVector<QPair<DListView *, MSortFilterProxyModel *> > vm;
  QMap<int,QString> vmIndex;
  QStringList *filterString;
//  QListView *listViewProgram;
//  QListView *listViewDoc;
  //!! only store listview etc
  QVBoxLayout *llayout;
  RecollModel *m_model;
  //    ResTableDetailArea *m_detail;
  int m_detaildocnum;
  Rcl::Doc m_detaildoc;
  int m_popcolumn;
  //    RclMain *m_rclmain;
  bool m_ismainres;
    //TODO multi
    DetailedWidget *dtw;
    int currentListViewIndex;
    int currentlistViewItemIndex;
};

#endif // RESLISTWIDGET_H

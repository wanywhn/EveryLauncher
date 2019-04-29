#ifndef RESLISTWIDGET_H
#define RESLISTWIDGET_H

#include <QAbstractListModel>
#include <QLabel>
#include <QListView>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QWidget>

#include "docseq.h"
#include "plaintorich.h"

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

typedef QString (FieldGetter)(const std::string &fldname,
                                 const Rcl::Doc &doc);

class RecollModel : public QAbstractListModel{

  Q_OBJECT
public:
    enum ModelRoles{
        Role_FILE_NAME = Qt::UserRole + 1,
        Role_LOCATION = Qt::UserRole + 2,
//        Role_MIMI_AUTHOR=Qt::UserRole+3,
        Role_ICON=Qt::UserRole+3,
        Role_FILE_SIMPLE_CONTENT = Qt::UserRole + 4,
        Role_MIME_TYPE = Qt::UserRole + 5,
        Role_RELEVANCY=Qt::UserRole+6,
//        Role_FILE_ICON=Qt::UserRole+6;
//        Role_FILE_EXEC=Qt::UserRole+7;
//        Role_DESKTOP_NAME=Qt::UserRole+8;
//        Role_DESKTOP_DESC=Qt::UserRole+9;
    };

public:
  RecollModel(const QStringList fields, QObject *parent = nullptr);

  virtual ~RecollModel();
  // Reimplemented methods
  virtual int rowCount(const QModelIndex & = QModelIndex()) const;
  virtual QVariant headerData(int col, Qt::Orientation orientation,
                              int role = Qt::DisplayRole) const;
  virtual QVariant data(const QModelIndex &index,
                        int role = Qt::DisplayRole) const;
  virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
  // Specific methods
  virtual void readDocSource();
  virtual void setDocSource(std::shared_ptr<DocSequence> nsource);
  virtual std::shared_ptr<DocSequence> getDocSource() { return m_source; }
//  virtual void deleteColumn(int);
  virtual const std::vector<std::string> &getFields() { return m_fields; }
  virtual const std::map<std::string, QString> &getAllFields() {
    return o_displayableFields;
  }
//  virtual void addColumn(int, const std::string &);
  // Some column name are aliases/translator for base document field
  // (ie: date, datetime->mtime). Help deal with this:
  virtual std::string baseField(const std::string &);

  // Ignore sort() call because
  virtual void setIgnoreSort(bool onoff) { m_ignoreSort = onoff; }

  friend class ResTable;

signals:
  void sortDataChanged(DocSeqSortSpec);

private:
  //    ResTable *m_table{nullptr};
  mutable std::shared_ptr<DocSequence> m_source;
  std::vector<std::string> m_fields;
  std::vector<FieldGetter *> m_getters;
  static std::map<std::string, QString> o_displayableFields;
  bool m_ignoreSort;
  FieldGetter *chooseGetter(const std::string &);
      HighlightData m_hdata;
};

// class ResTablePager;
class QUrl;
// class RclMain;

class ResTable : public QWidget {
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
public slots:
  virtual void onTableView_currentChanged(const QModelIndex &);
  virtual void on_tableView_entered(const QModelIndex &index);
  virtual void setDocSource(std::shared_ptr<DocSequence> nsource);
  virtual void resetSource();
  virtual void readDocSource(bool resetPos = true);
//  virtual void onSortDataChanged(DocSeqSortSpec);
  virtual void makeRowVisible(int row);
  virtual void takeFocus();
  void moveToNextResoule();

  //    friend class ResTablePager;
  //    friend class ResTableDetailArea;
private:
  QVector<QPair<QListView *, QSortFilterProxyModel *> > vm;
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
    QLabel *detailedWidget;
    int currentListViewIndex;
    int currentlistViewItemIndex;
};

#endif // RESLISTWIDGET_H

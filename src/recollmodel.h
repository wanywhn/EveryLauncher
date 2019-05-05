#ifndef RECOLLMODEL_H
#define RECOLLMODEL_H

#include <QAbstractListModel>
#include <docseq.h>

#include <bits/shared_ptr.h>
extern RclConfig *theconfig;
typedef QString (FieldGetter)(const std::string &fldname,
                                 const Rcl::Doc &doc);

  class ResTable;

class RecollModel : public QAbstractListModel{

  Q_OBJECT
public:
    enum ModelRoles{
        Role_FILE_NAME = Qt::UserRole + 1,
        Role_LOCATION = Qt::UserRole + 2,
//        Role_MIMI_AUTHOR=Qt::UserRole+3,
        Role_ICON_PATH=Qt::UserRole+3,
        Role_FILE_SIMPLE_CONTENT = Qt::UserRole + 4,
        Role_MIME_TYPE = Qt::UserRole + 5,
        Role_RELEVANCY=Qt::UserRole+6,
        Role_APP_COMMENT=Qt::UserRole+7,
        Role_APP_NAME=Qt::UserRole+8,
        Role_VIEW_TYPE=Qt::UserRole+9,
        Role_NODISPLAY=Qt::UserRole+10,
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


signals:
  void sortDataChanged(DocSeqSortSpec);

  friend class ResTable;
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


#endif // RECOLLMODEL_H

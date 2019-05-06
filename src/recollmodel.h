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
        Role_ICON_PATH=Qt::UserRole+3,
        Role_FILE_SIMPLE_CONTENT = Qt::UserRole + 4,
        Role_MIME_TYPE = Qt::UserRole + 5,
        Role_RELEVANCY=Qt::UserRole+6,
        Role_APP_COMMENT=Qt::UserRole+7,
        Role_APP_NAME=Qt::UserRole+8,
        Role_VIEW_TYPE=Qt::UserRole+9,
        Role_NODISPLAY=Qt::UserRole+10,
    };

public:
    explicit RecollModel(QStringList fields, QObject *parent = nullptr);

    ~RecollModel() override = default;

    // Reimplemented methods
    int rowCount(const QModelIndex &) const override;

    QVariant headerData(int col, Qt::Orientation orientation,
                              int role ) const override;

    QVariant data(const QModelIndex &index,
                        int role ) const override;
  virtual void readDocSource();
  virtual void setDocSource(std::shared_ptr<DocSequence> nsource);
  virtual std::shared_ptr<DocSequence> getDocSource() { return m_source; }
  virtual const std::vector<std::string> &getFields() { return m_fields; }
  virtual const std::map<std::string, QString> &getAllFields() {
    return o_displayableFields;
  }


signals:
  void sortDataChanged(DocSeqSortSpec);

  friend class ResTable;
private:
  mutable std::shared_ptr<DocSequence> m_source;
  std::vector<std::string> m_fields;
  std::vector<FieldGetter *> m_getters;
  static std::map<std::string, QString> o_displayableFields;
  FieldGetter *chooseGetter(const std::string &);
      HighlightData m_hdata;
};


#endif // RECOLLMODEL_H

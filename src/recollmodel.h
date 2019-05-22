#ifndef RECOLLMODEL_H
#define RECOLLMODEL_H

#include <QAbstractListModel>
#include <docseq.h>

#include <bits/shared_ptr.h>
#include "ELModelInterface.h"

extern RclConfig *theconfig;
typedef QString (FieldGetter)(const std::string &fldname,
                                 const Rcl::Doc &doc);

  class ResWidget;

class RecollModel : public QAbstractListModel,public ELModelInterface{

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
        Role_APP_VERSION=Qt::UserRole+11,
    };

public:
    explicit RecollModel(QObject *parent = nullptr);

    ~RecollModel() override = default;

    // Reimplemented methods
    int rowCount(const QModelIndex &) const override;


    QVariant data(const QModelIndex &index,
                        int role ) const override;
    void search(std::string &str) override;
    void setFilterSpec(DocSeqFiltSpec &spec);
private:
  virtual void readDocSource();
  virtual void setDocSource(std::shared_ptr<DocSequence> nsource);
  virtual std::shared_ptr<DocSequence> getDocSource() { return m_source; }
  virtual const std::map<std::string, QString> &getAllFields() {
    return o_displayableFields;
  }
private :

    virtual void initiateQuery();

public slots:
    virtual void resetSource();

signals:
  void sortDataChanged(DocSeqSortSpec);
  void restultReady();

private:


    bool m_queryActive{false};
    bool m_indexed{false};
  mutable std::shared_ptr<DocSequence> m_source;
  static std::map<std::string, QString> o_displayableFields;
  FieldGetter *chooseGetter(const std::string &);
      HighlightData m_hdata;
};


#endif // RECOLLMODEL_H

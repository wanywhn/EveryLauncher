#ifndef RECOLLMODEL_H
#define RECOLLMODEL_H

#include <QAbstractListModel>
#include <docseq.h>

#include "Model/ELModelInterface.h"

extern RclConfig *theconfig;
typedef QString (FieldGetter)(const std::string &fldname,
                                 const Rcl::Doc &doc);

class RecollModel :public ELModelInterface{

  Q_OBJECT
public:


public:
    explicit RecollModel(const QString &filed, QObject *parent = nullptr);

    ~RecollModel() override = default;

    // Reimplemented methods
    int rowCount(const QModelIndex &) const override;


    QVariant data(const QModelIndex &index,
                        int role ) const override;
    void search(QString &qstr) override;

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

    void sourceChanged() override;

    void clearSource() override;


private:


    bool m_queryActive{false};
    bool m_indexed{false};
  mutable std::shared_ptr<DocSequence> m_source;
  static std::map<std::string, QString> o_displayableFields;
  FieldGetter *chooseGetter(const std::string &);
      HighlightData m_hdata;

    void getHighlight(const vector<string> &vs, QStringList &sl) const;

    void getImgUrl(const Rcl::Doc &doc, QVariant &var, QUrl &u) const;
    DocSeqSortSpec dsss;
    DocSeqFiltSpec dsfs;
};


#endif // RECOLLMODEL_H

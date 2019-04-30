#ifndef SEARCHLINE_H
#define SEARCHLINE_H

#include <QObject>

#include <string>

#include <QAbstractListModel>
#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPixmap>
#include <QVariant>
#include <QWidget>

class QTimer;

#include "searchdata.h"
#include <memory>

struct SSearchDef;
extern std::shared_ptr<Rcl::Db> rcldb;
class RclCompleterModel : public QAbstractListModel {

  Q_OBJECT

public:
  RclCompleterModel(QWidget *parent = 0) : QAbstractListModel(parent) {
    init();
  }
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
public slots:
  virtual void onPartialWord(int, const QString &, const QString &);

private:
  void init();
  vector<QString> currentlist;
  int firstfromindex;
  QPixmap clockPixmap;
  QPixmap interroPixmap;
};

class MLineEdit : public QLineEdit {
  Q_OBJECT
public:
  MLineEdit(QWidget *parent);
  ~MLineEdit() {}

  // QObject interface
public:
  bool event(QEvent *event);

signals:
  void tabPressed();
  void stabPressed();

//  void textChanged();
};

class SSearch : public QWidget {
  Q_OBJECT

public:
  // The values MUST NOT change, there are assumptions about them in
  // different parts of the code
  enum SSearchType { SST_ANY = 0, SST_ALL = 1, SST_FNM = 2, SST_LANG = 3 };

  SSearch(QWidget *parent = 0, const char * = 0);

  virtual bool hasSearchString();
  virtual void setPrefs();
  // Return last performed search as XML text.
  // virtual std::string asXML();
  // Restore ssearch UI from saved search
  //    virtual bool fromXML(const SSearchDef& fxml);
  virtual QString currentText();

public slots:
  virtual void searchTypeChanged(int);
  virtual void setSearchString(const QString &text);
  virtual void startSimpleSearch();
  virtual void addTerm(QString);
  virtual void onWordReplace(const QString &, const QString &);
  virtual void takeFocus();
  // Forget current entry and any state (history)
  virtual void clearAll();

private slots:
  virtual void searchTextChanged(const QString &);
  virtual void searchTextEdited(const QString &);
  virtual void onCompletionActivated(const QString &);
  virtual void restoreText();
  virtual void onHistoryClicked();

signals:
  void startSearch(std::shared_ptr<Rcl::SearchData>, bool);
  void setDescription(QString);
  void clearSearch();
  void partialWord(int, const QString &text, const QString &partial);
  void tabPressed();
  void stabPressed();
  void returnPressed();

private:
  void init_ui();
  void init_conn();

private:
  int getPartialWord(QString &word);
  bool startSimpleSearch(const string &q);
  MLineEdit *queryText;

  /* We save multiword entries because the completer replaces them with
     the completion */
  QString m_savedEditText;
  /* Saved xml version of the search, as we start it */
  // std::string m_xml;
  RclCompleterModel *m_completermodel{nullptr};
};

#endif // SEARCHLINE_H

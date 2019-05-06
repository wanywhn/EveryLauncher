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
#include <memory>

class QTimer;

#include "searchdata.h"

extern RclConfig *theconfig;
extern std::shared_ptr<Rcl::Db> rcldb;
/*
 * 将来用作搜索词不全，比如程序名，网络搜索词
 */
class KeyWordsCompleterModel : public QAbstractListModel {

  Q_OBJECT

public:
    explicit KeyWordsCompleterModel(QWidget *parent = 0) : QAbstractListModel(parent) {
    init();
  }
  int rowCount(const QModelIndex &) const override;
  QVariant data(const QModelIndex &index,
                int role ) const override;
public slots:
  virtual void onPartialWord(int, const QString &, const QString &);

private:
  void init();
  vector<QString> currentlist;
  int firstfromindex{};
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

class SearchWidget : public QWidget {
  Q_OBJECT

public:

  SearchWidget(QWidget *parent = 0, const char * = 0);

  virtual bool hasSearchString();

  virtual QString currentText();

public slots:

    virtual void setSearchString(const QString &text);
  virtual void startSimpleSearch();

    virtual void onWordReplace(const QString &, const QString &);
  virtual void takeFocus();
  virtual void clearAll();

private slots:
  virtual void searchTextChanged(const QString &);
  virtual void searchTextEdited(const QString &);
  virtual void onCompletionActivated(const QString &);
  virtual void restoreText();

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

  QString m_savedEditText;
  KeyWordsCompleterModel *m_completermodel{nullptr};
};

#endif // SEARCHLINE_H

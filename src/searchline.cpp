#include "searchline.h"
#include <QAbstractItemView>
#include <QAbstractListModel>
#include <QCompleter>
#include <QDesktopServices>
#include <QKeySequence>
#include <QListView>
#include <QModelIndex>
#include <QSettings>
#include <QShortcut>
#include <QTimer>
#include <memory>
#include <qapplication.h>
#include <qcombobox.h>
#include <qevent.h>
#include <qinputdialog.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qvariant.h>
#include <qwhatsthis.h>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "log.h"
#include "rcldb.h"
#include "searchdata.h"
#include "smallut.h"
#include "textsplit.h"
#include "wasatorcl.h"

using namespace std;

// Max search history matches displayed in completer
static const int maxhistmatch = 10;
// Max db term matches fetched from the index
static const int maxdbtermmatch = 20;
// Visible rows for the completer listview
static const int completervisibleitems = 20;

void KeyWordsCompleterModel::init() {}

int KeyWordsCompleterModel::rowCount(const QModelIndex &) const {
  return currentlist.size();
}

QVariant KeyWordsCompleterModel::data(const QModelIndex &index,
                                      int role) const {
  if (role != Qt::DisplayRole && role != Qt::EditRole &&
      role != Qt::DecorationRole) {
    return QVariant();
  }
  if (index.row() < 0 || index.row() >= int(currentlist.size())) {
    return QVariant();
  }

  if (role == Qt::DecorationRole) {
    return index.row() < firstfromindex ? QVariant(clockPixmap)
                                        : QVariant(interroPixmap);
  } else {
    return QVariant(currentlist[index.row()]);
  }
}

void KeyWordsCompleterModel::onPartialWord(int tp, const QString &_qtext,
                                           const QString &qpartial) {
  //  string partial = qpartial.toStdString();
  //  QString qtext = _qtext.trimmed();
  //  bool onlyspace = qtext.isEmpty();

  currentlist.clear();
  beginResetModel();
  int histmatch = 0;

  firstfromindex = currentlist.size();

  /*
  if (!qpartial.trimmed().isEmpty()) {
    Rcl::TermMatchResult rclmatches;
    if (!rcldb->termMatch(Rcl::Db::ET_WILD, string(), partial + "*", rclmatches,
                          maxdbtermmatch)) {
      LOGDEB1("KeyWordsCompleterModel: termMatch failed: [" << partial + "*"
                                                       << "]\n");
    } else {
      LOGDEB1("KeyWordsCompleterModel: termMatch cnt: " <<
  rclmatches.entries.size()
                                                   << endl);
    }
    for (const auto &entry : rclmatches.entries) {
      LOGDEB1("KeyWordsCompleterModel: match " << entry.term << endl);
      string data = entry.term;
      currentlist.push_back(QString::fromStdString(data));
    }
  }
   */
  endResetModel();
}

SearchWidget::SearchWidget(QWidget *parent, const char *) : QWidget(parent) {
  queryText = new MLineEdit(this);
  m_completermodel = new KeyWordsCompleterModel(this);
  //  queryText->installEventFilter(this);
  QTimer::singleShot(0, queryText, SLOT(setFocus()));
  this->btnSearch = new QPushButton(tr("搜索"), this);
  init_ui();
  init_conn();
}

void SearchWidget::init_conn() {
  //    connect(queryText,&QLineEdit::textEdited,this,static_cast<void(SearchWidget::*)(void)>(&SearchWidget::startSimpleSearch));
  connect(queryText, &QLineEdit::textChanged, this,
          &SearchWidget::searchTextChanged);
  connect(queryText, &QLineEdit::textEdited, this,
          &SearchWidget::searchTextEdited);
  connect(queryText, &MLineEdit::returnPressed, [this]() {
    if (normalInputState) {
      emit this->returnPressed();
      return;
    }
    if (searchUrl.isEmpty()) {
      return;
    }
    auto qt = this->queryText->text();
    // left three char is "xx "
    auto content = qt.right(qt.length() - 3).trimmed();

    if (content.isEmpty()) {
      return;
    }
    QDesktopServices::openUrl(QUrl(searchUrl + content.replace(" ", "+")));
  });

  auto completer = new QCompleter(m_completermodel, this);
  completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
  completer->setFilterMode(Qt::MatchContains);
  completer->setCaseSensitivity(Qt::CaseInsensitive);
  completer->setMaxVisibleItems(completervisibleitems);
  queryText->setCompleter(completer);
  //  connect(this, &SearchWidget::partialWord, m_completermodel,
  //          &KeyWordsCompleterModel::onPartialWord);
  connect(completer, SIGNAL(activated(const QString &)), this,
          SLOT(onCompletionActivated(const QString &)));
  //    auto s=new QShortcut(tr("ESC"),queryText);
  //    connect(s,&QShortcut::activated,this,&SearchWidget::clearAll);
  connect(this->queryText, &MLineEdit::tabPressed, [this]() {
    if (normalInputState)
      emit this->tabPressed();
  });
  //  connect(this->queryText,&MLineEdit::stabPressed,this,&SearchWidget::stabPressed);
  connect(this->btnSearch, &QPushButton::clicked, this,
          &SearchWidget::prepareSimpleSearch);
}

void SearchWidget::init_input_state() { normalInputState = true; }

void SearchWidget::takeFocus() {
  LOGDEB1("SearchWidget: take focus\n");
  queryText->setFocus(Qt::ShortcutFocusReason);
  queryText->selectAll();
}

QString SearchWidget::currentText() { return queryText->text(); }

void SearchWidget::clearAll() {
  queryText->clear();
  init_input_state();
  emit clearSearch();
}

// onCompletionActivated() is called when an entry is selected in the
// popup, but the edit text is going to be replaced in any case if
// there is a current match (we can't prevent it in the signal). If
// there is no match (e.g. the user clicked the history button and
// selected an entry), the query text will not be set.
// So:
//  - We set the query text to the popup activation value in all cases
//  - We schedule a callback to set the text to what we want (which is the
//    concatenation of the user entry before the current partial word and the
//    pop up data.
//  - Note that a history click will replace a current partial word,
//    so that the effect is different if there is a space at the end
//    of the entry or not: pure concatenation vs replacement of the
//    last (partial) word.
void SearchWidget::restoreText() {
  LOGDEB("SearchWidget::restoreText: savedEdit: "
         << m_savedEditText.toStdString() << endl);
  if (!m_savedEditText.trimmed().isEmpty()) {
    // If the popup text begins with the saved text, just let it replace
    if (currentText().lastIndexOf(m_savedEditText) != 0) {
      queryText->setText(m_savedEditText.trimmed() + " " + currentText());
    }
    m_savedEditText = "";
  }
  queryText->setFocus();
  //    if (prefs.ssearchStartOnComplete) {
  //        QTimer::singleShot(0, this, SLOT(startSimpleSearch()));
  //    }
}
void SearchWidget::onCompletionActivated(const QString &text) {
  queryText->setText(text);
  QTimer::singleShot(0, this, SLOT(restoreText()));
}

void SearchWidget::init_ui() {
  auto hLayout = new QHBoxLayout();
  this->setLayout(hLayout);

  hLayout->addWidget(this->queryText);
  hLayout->addWidget(btnSearch);
}

void SearchWidget::searchTextEdited(const QString &text) {
  // TODO prepare compe
  QString pword = text;
  int cs = getPartialWord(pword);

  //  m_savedEditText = text.left(cs);
  //  if (cs >= 0) {
  //            emit partialWord(tp, currentText(), pword);
  //  } else {
  //            emit partialWord(0,pword, " ");
  //  }
}

void SearchWidget::searchTextChanged(const QString &text) {
  LOGDEB1("SearchWidget::searchTextChanged: text [" << qs2u8s(text) << "]\n");

  // TODO it's better to insert some file into xapian to impl search
  if (text.size() < 3) {
    normalInputState = true;
  }
  if (text.size() == 3) {
    if (text.trimmed().size() == 2) {
      QSettings sett;
      sett.beginGroup("SearchPrefix");
      searchUrl = sett.value(text.trimmed()).toString();
      if (!searchUrl.isEmpty()) {
        normalInputState = false;
      }
      sett.endGroup();
      return;
    }
    normalInputState = true;
  }
  if (text.trimmed().size() >= 3 && normalInputState) {
    prepareSimpleSearch();
  } else {
    emit clearSearch();
  }
}

void SearchWidget::prepareSimpleSearch() {
  if (queryText->completer()->popup()->isVisible()) {
    return;
  }
  auto str = queryText->text();
  if (str.trimmed().isEmpty()) {
    emit clearSearch();
    return;
  }
  QString s("");
  for (auto tmp : str) {
    if ((tmp >= 'a' && tmp <= 'z') || (tmp >= 'A' && tmp <= 'Z')) {
      s += tmp + QString("*");
    } else {
      s += tmp;
    }
  }
  if(s.isEmpty()){
    return ;
  }

  emit startSearch(s.toStdString());
}

bool SearchWidget::hasSearchString() { return !currentText().isEmpty(); }

void SearchWidget::onWordReplace(const QString &o, const QString &n) {
  LOGDEB("SearchWidget::onWordReplace: o [" << o.toStdString() << "] n ["
                                            << n.toStdString() << "]\n");
  QString txt = currentText();
  QRegExp exp = QRegExp(QString("\\b") + o + QString("\\b"));
  exp.setCaseSensitivity(Qt::CaseInsensitive);
  txt.replace(exp, n);
  queryText->setText(txt);
  Qt::KeyboardModifiers mods = QApplication::keyboardModifiers();
  if (mods == Qt::NoModifier)
    prepareSimpleSearch();
}

int SearchWidget::getPartialWord(QString &word) {}

MLineEdit::MLineEdit(QWidget *parent) : QLineEdit(parent) {}

bool MLineEdit::event(QEvent *event) {
  if (event->type() != QEvent::KeyPress) {
    return QLineEdit::event(event);
  }
  auto kev = dynamic_cast<QKeyEvent *>(event);
  if (kev->key() != Qt::Key_Tab) {
    return QLineEdit::event(event);
  }
  emit tabPressed();
  //    qDebug() << "tab";
  return true;
}

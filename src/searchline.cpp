#include "searchline.h"

#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <QAbstractItemView>
#include <QAbstractListModel>
#include <QCompleter>
#include <QKeySequence>
#include <QListView>
#include <QModelIndex>
#include <QShortcut>
#include <QTimer>
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

void RclCompleterModel::init() {
  //    if (!clockPixmap.load(":/images/clock.png") ||
  //        !interroPixmap.load(":/images/interro.png")) {
  //        LOGERR("SSearch: pixmap loading failed\n");
  //    }
}

int RclCompleterModel::rowCount(const QModelIndex &) const {
  LOGDEB1("RclCompleterModel::rowCount: " << currentlist.size() << "\n");
  return currentlist.size();
}

QVariant RclCompleterModel::data(const QModelIndex &index, int role) const {
  LOGDEB1("RclCompleterModel::data: row: " << index.row() << " role " << role
                                           << "\n");
  if (role != Qt::DisplayRole && role != Qt::EditRole &&
      role != Qt::DecorationRole) {
    return QVariant();
  }
  if (index.row() < 0 || index.row() >= int(currentlist.size())) {
    return QVariant();
  }

  if (role == Qt::DecorationRole) {
    LOGDEB1("RclCompleterModel::data: returning pixmap\n");
    return index.row() < firstfromindex ? QVariant(clockPixmap)
                                        : QVariant(interroPixmap);
  } else {
    LOGDEB1("RclCompleterModel::data: return: "
            << qs2u8s(currentlist[index.row()]) << endl);
    return QVariant(currentlist[index.row()]);
  }
}

void RclCompleterModel::onPartialWord(int tp, const QString &_qtext,
                                      const QString &qpartial) {
  string partial = qpartial.toStdString();
  QString qtext = _qtext.trimmed();
  bool onlyspace = qtext.isEmpty();
  LOGDEB1("RclCompleterModel::onPartialWord: [" << partial << "] onlyspace "
                                                << onlyspace << "\n");

  currentlist.clear();
  beginResetModel();
  //    if ((prefs.ssearchNoComplete && !onlyspace) || tp == SSearch::SST_FNM) {
  // Nocomplete: only look at history by entering space
  // Filename: no completion for now. We'd need to termatch with
  // the right prefix?
  //        endResetModel();
  //        return;
  //    }

  int histmatch = 0;
  // Look for matches between the full entry and the search history
  // (anywhere in the string)
  /*
  for (int i = 0; i < prefs.ssearchHistory.count(); i++) {
      LOGDEB1("[" << qs2u8s(prefs.ssearchHistory[i]) << "] contains [" <<
              qs2u8s(qtext) << "] ?\n");
      // If there is current text, only show a limited count of
      // matching entries, else show the full history.
      if (onlyspace ||
          prefs.ssearchHistory[i].contains(qtext, Qt::CaseInsensitive)) {
          currentlist.push_back(prefs.ssearchHistory[i]);
          if (!onlyspace && ++histmatch >= maxhistmatch)
              break;
      }
  }
  */
  firstfromindex = currentlist.size();

  // Look for Recoll terms beginning with the partial word
  if (!qpartial.trimmed().isEmpty()) {
    Rcl::TermMatchResult rclmatches;
    if (!rcldb->termMatch(Rcl::Db::ET_WILD, string(), partial + "*", rclmatches,
                          maxdbtermmatch)) {
      LOGDEB1("RclCompleterModel: termMatch failed: [" << partial + "*"
                                                       << "]\n");
    } else {
      LOGDEB1("RclCompleterModel: termMatch cnt: " << rclmatches.entries.size()
                                                   << endl);
    }
    for (const auto &entry : rclmatches.entries) {
      LOGDEB1("RclCompleterModel: match " << entry.term << endl);
      string data = entry.term;
      currentlist.push_back(QString::fromStdString(data));
    }
  }
  endResetModel();
}

SSearch::SSearch(QWidget *parent, const char *) : QWidget(parent) {
  queryText = new MLineEdit(this);
  queryText->installEventFilter(this);
  QTimer::singleShot(0, queryText, SLOT(setFocus()));
  init_ui();
  init_conn();
}

void SSearch::init_conn() {
  // See enum in .h and keep in order !
//  connect(queryText, SIGNAL(returnPressed()), this, SLOT(startSimpleSearch()));
//  connect(queryText,SLOT(textChanged(const QString &)),this,SLOT(startSimpleSearch()));
    connect(queryText,&QLineEdit::textChanged,this,static_cast<void(SSearch::*)(void)>(&SSearch::startSimpleSearch));
//  connect(queryText, &QLineEdit::textChanged, this,
//          &SSearch::searchTextChanged);
//  connect(queryText, &QLineEdit::textEdited, this, &SSearch::searchTextEdited);
  connect(queryText,&MLineEdit::returnPressed,this,&SSearch::returnPressed);

  m_completermodel = new RclCompleterModel(this);
  QCompleter *completer = new QCompleter(m_completermodel, this);
  completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
  completer->setFilterMode(Qt::MatchContains);
  completer->setCaseSensitivity(Qt::CaseInsensitive);
  completer->setMaxVisibleItems(completervisibleitems);
  queryText->setCompleter(completer);
  connect(this, &SSearch::partialWord, m_completermodel,
          &RclCompleterModel::onPartialWord);
  connect(completer, SIGNAL(activated(const QString &)), this,
          SLOT(onCompletionActivated(const QString &)));
  //    auto s=new QShortcut(tr("ESC"),queryText);
  //    connect(s,&QShortcut::activated,this,&SSearch::clearAll);
  connect(this->queryText,&MLineEdit::tabPressed,this,&SSearch::tabPressed);
  connect(this->queryText,&MLineEdit::stabPressed,this,&SSearch::stabPressed);
}

void SSearch::takeFocus() {
  LOGDEB1("SSearch: take focus\n");
  queryText->setFocus(Qt::ShortcutFocusReason);
  // If the focus was already in the search entry, the text is not selected.
  // Do it for consistency
  queryText->selectAll();
}

QString SSearch::currentText() { return queryText->text(); }

void SSearch::clearAll() { queryText->clear();emit clearSearch(); }

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
void SSearch::restoreText() {
  LOGDEB("SSearch::restoreText: savedEdit: " << m_savedEditText.toStdString()
                                             << endl);
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
void SSearch::onCompletionActivated(const QString &text) {
  //    LOGDEB("SSearch::onCompletionActivated: queryText [" <<
  //           qs2u8s(currentText()) << "] text [" << qs2u8s(text) << "]\n");
  queryText->setText(text);
  QTimer::singleShot(0, this, SLOT(restoreText()));
}

void SSearch::onHistoryClicked() {
  if (m_completermodel) {
    m_completermodel->onPartialWord(SST_LANG, "", "");
    queryText->completer()->complete();
  }
}

void SSearch::init_ui() {
  auto hLayout = new QHBoxLayout();
  this->setLayout(hLayout);

  hLayout->addWidget(this->queryText);
}

void SSearch::searchTextEdited(const QString &text) {
  LOGDEB1("SSearch::searchTextEdited: text [" << qs2u8s(text) << "]\n");
  QString pword;
  int cs = getPartialWord(pword);
  //    int tp = searchTypCMB->currentIndex();

  m_savedEditText = text.left(cs);
  LOGDEB1("SSearch::searchTextEdited: cs " << cs << " pword [" << qs2u8s(pword)
                                           << "] savedEditText ["
                                           << qs2u8s(m_savedEditText) << "]\n");
  if (cs >= 0) {
    //        emit partialWord(tp, currentText(), pword);
  } else {
    //        emit partialWord(tp, currentText(), " ");
  }
}

void SSearch::searchTextChanged(const QString &text) {
  LOGDEB1("SSearch::searchTextChanged: text [" << qs2u8s(text) << "]\n");

  if (text.isEmpty()) {
    queryText->setFocus();
    emit clearSearch();
  } else {
  }
}

void SSearch::searchTypeChanged(int typ) {
  LOGDEB1("Search type now " << typ << "\n");
  // Adjust context help
  if (typ == SST_LANG) {
    //        HelpClient::installMap((const char *)this->objectName().toUtf8(),
    //                               "RCL.SEARCH.LANG");
  } else {
    //        HelpClient::installMap((const char *)this->objectName().toUtf8(),
    //                               "RCL.SEARCH.GUI.SIMPLE");
  }
  // Also fix tooltips
  switch (typ) {
  case SST_LANG:
    queryText->setToolTip(
        tr("Enter query language expression. Cheat sheet:<br>\n"
           "<i>term1 term2</i> : 'term1' and 'term2' in any field.<br>\n"
           "<i>field:term1</i> : 'term1' in field 'field'.<br>\n"
           " Standard field names/synonyms:<br>\n"
           "  title/subject/caption, author/from, recipient/to, filename, "
           "ext.<br>\n"
           " Pseudo-fields: dir, mime/format, type/rclcat, date, size.<br>\n"
           " Two date interval exemples: 2009-03-01/2009-05-20  "
           "2009-03-01/P2M.<br>\n"
           "<i>term1 term2 OR term3</i> : term1 AND (term2 OR term3).<br>\n"
           "  You can use parentheses to make things clearer.<br>\n"
           "<i>\"term1 term2\"</i> : phrase (must occur exactly). Possible "
           "modifiers:<br>\n"
           "<i>\"term1 term2\"p</i> : unordered proximity search with default "
           "distance.<br>\n"
           "Use <b>Show Query</b> link when in doubt about result and see "
           "manual (&lt;F1>) for more detail.\n"));
    break;
  case SST_FNM:
    queryText->setToolTip(tr("Enter file name wildcard expression."));
    break;
  case SST_ANY:
  case SST_ALL:
  default:
    queryText->setToolTip(tr("Enter search terms here. Type ESC SPC for "
                             "completions of current term."));
  }
}

void SSearch::startSimpleSearch() {
  if (queryText->completer()->popup()->isVisible()) {
    return;
  }
  auto str=queryText->text();
  if(str.trimmed().isEmpty()){
      emit clearSearch();
      return ;

  }
  QString s("");
  for(auto tmp:str){
      if((tmp>='a'&&tmp<='z')||(tmp>='A'&&tmp<='Z')){
        s+=tmp+QString("*");
      }else{
          s+=tmp;
      }
  }
  string u8=s.toStdString();
  trimstring(u8);
  if (u8.length() == 0)
    return;

  if (!startSimpleSearch(u8))
    return;

}

void SSearch::setPrefs() {}

bool SSearch::startSimpleSearch(const string &u8) {
  LOGDEB("SSearch::startSimpleSearch(" << u8 << ")\n");
  // TODO
  string stemlang = "english"; // prefs.stemlang();




  Rcl::SearchData *sdata = nullptr;

  std::string reason;
  sdata = wasaStringToRcl(theconfig, stemlang, u8, reason);

  std::shared_ptr<Rcl::SearchData> rsdata(sdata);
  emit setDescription(QString::fromStdString(u8));
  emit startSearch(rsdata, true);
  return true;
}


void SSearch::setSearchString(const QString &txt) { queryText->setText(txt); }

bool SSearch::hasSearchString() { return !currentText().isEmpty(); }

// Add term to simple search. Term comes out of double-click in
// reslist or preview.
// It would probably be better to cleanup in preview.ui.h and
// reslist.cpp and do the proper html stuff in the latter case
// (which is different because it format is explicit richtext
// instead of auto as for preview, needed because it's built by
// fragments?).
static const char *punct = " \t()<>\"'[]{}!^*.,:;\n\r";
void SSearch::addTerm(QString term) {
  LOGDEB("SSearch::AddTerm: [" << term.toStdString() << "]\n");
  string t = (const char *)term.toUtf8();
  string::size_type pos = t.find_last_not_of(punct);
  if (pos == string::npos)
    return;
  t = t.substr(0, pos + 1);
  pos = t.find_first_not_of(punct);
  if (pos != string::npos)
    t = t.substr(pos);
  if (t.empty())
    return;
  term = QString::fromUtf8(t.c_str());

  QString text = currentText();
  text += QString::fromLatin1(" ") + term;
  queryText->setText(text);
}

void SSearch::onWordReplace(const QString &o, const QString &n) {
  LOGDEB("SSearch::onWordReplace: o [" << o.toStdString() << "] n ["
                                       << n.toStdString() << "]\n");
  QString txt = currentText();
  QRegExp exp = QRegExp(QString("\\b") + o + QString("\\b"));
  exp.setCaseSensitivity(Qt::CaseInsensitive);
  txt.replace(exp, n);
  queryText->setText(txt);
  Qt::KeyboardModifiers mods = QApplication::keyboardModifiers();
  if (mods == Qt::NoModifier)
    startSimpleSearch();
}

// If text does not end with space, return last (partial) word and >0
// else return -1
int SSearch::getPartialWord(QString &word) {
  // Extract last word in text
  QString txt = currentText();
  if (txt.isEmpty()) {
    return -1;
  }
  int lstidx = txt.size() - 1;

  // If the input ends with a space or dquote (phrase input), or
  // dquote+qualifiers, no partial word.
  if (txt[lstidx] == ' ') {
    return -1;
  }
  int cs = txt.lastIndexOf("\"");
  if (cs > 0) {
    bool dquoteToEndNoSpace{true};
    for (int i = cs; i <= lstidx; i++) {
      if (txt[i] == ' ') {
        dquoteToEndNoSpace = false;
        break;
      }
    }
    if (dquoteToEndNoSpace) {
      return -1;
    }
  }

  cs = txt.lastIndexOf(" ");
  if (cs < 0)
    cs = 0;
  else
    cs++;
  word = txt.right(txt.size() - cs);
  return cs;
}

MLineEdit::MLineEdit(QWidget *parent) : QLineEdit(parent) {
    //      connect(this,&QLineEdit::textChanged,this,&MLineEdit::textChanged);
}

bool MLineEdit::event(QEvent *event) {
    if (event->type() != QEvent::KeyPress) {
        return QLineEdit::event(event);
    }
    auto kev = static_cast<QKeyEvent *>(event);
    if (kev->key() != Qt::Key_Tab) {
        return QLineEdit::event(event);
    }
    emit tabPressed();
    //    qDebug() << "tab";
    return true;
    //    return QLineEdit::event(event);
}

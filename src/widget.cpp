#include "widget.h"
#include "ui_widget.h"

#include <QMessageBox>
#include <QProgressDialog>
#include <QThread>
#include <QVBoxLayout>
#include <docseqdb.h>

extern std::shared_ptr<Rcl::Db> rcldb;
extern bool maybeOpenDb(string &reason, bool force, bool *maindberror);
extern RclConfig *theconfig;
// Start a db query and set the reslist docsource
void Widget::startSearch(std::shared_ptr<Rcl::SearchData> sdata,
                         bool issimple) {
  //    LOGDEB("RclMain::startSearch. Indexing " << (m_idxproc?"on":"off") <<
  //           " Active " << m_queryActive << "\n");
  if (m_queryActive) {
    //    LOGDEB("startSearch: already active\n");
    return;
  }
  m_queryActive = true;
  restable->setEnabled(false);
  m_source = std::shared_ptr<DocSequence>();

  //    m_searchIsSimple = issimple;

  // The db may have been closed at the end of indexing
  string reason;
  // If indexing is being performed, we reopen the db at each query.
  bool b;
  if (!maybeOpenDb(reason, m_idxproc != 0, &b)) {
    QMessageBox::critical(0, "Recoll", QString(reason.c_str()),
                          QMessageBox::Ok);
    m_queryActive = false;
    restable->setEnabled(true);
    return;
  }

  //    if (prefs.synFileEnable && !prefs.synFile.isEmpty()) {
  //        string sf = (const char *)prefs.synFile.toLocal8Bit();
  //        if (!rcldb->setSynGroupsFile(sf)) {
  //            QMessageBox::warning(0, "Recoll",
  //                                 tr("Can't set synonyms file (parse
  //                                 error?)"));
  //            return;
  //        }
  //    } else {
  //  rcldb->setSynGroupsFile("");
  //    }

  Rcl::Query *query = new Rcl::Query(rcldb.get());
  //  query->setQuery(sdata);
  query->setCollapseDuplicates(true);

  //    curPreview = 0;
  DocSequenceDb *src =
      new DocSequenceDb(/*rcldb,*/ std::shared_ptr<Rcl::Query>(query),
                        string(tr("Query results").toUtf8()), sdata);
  //      new DocSequenceDb(rcldb, std::shared_ptr<Rcl::Query>(query),
  //                        string(tr("Query results").toUtf8()), sdata);
  src->setAbstractParams(true, false);
  //                           prefs.queryReplaceAbstract);
  m_source = std::shared_ptr<DocSequence>(src);
  DocSeqSortSpec dsss;
  DocSeqFiltSpec dsfs;
  m_source->setSortSpec(dsss);
  m_source->setFiltSpec(dsfs);

  emit docSourceChanged(m_source);
  //    emit sortDataChanged(m_sortspec);
  initiateQuery();
}

class QueryThread : public QThread {
  std::shared_ptr<DocSequence> m_source;

public:
  QueryThread(std::shared_ptr<DocSequence> source) : m_source(source) {}
  ~QueryThread() {}
  virtual void run() { cnt = m_source->getResCnt(); }
  int cnt;
};

void Widget::initiateQuery() {
  if (!m_source)
    return;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  QueryThread qthr(m_source);
  qthr.start();

  QProgressDialog progress(this);
  progress.setLabelText(tr("Query in progress.<br>"
                           "Due to limitations of the indexing library,<br>"
                           "cancelling will exit the program"));
  progress.setWindowModality(Qt::WindowModal);
  progress.setRange(0, 0);

  // For some reason setMinimumDuration() does not seem to work with
  // a busy dialog (range 0,0) Have to call progress.show() inside
  // the loop.
  // progress.setMinimumDuration(2000);
  // Also the multiple processEvents() seem to improve the responsiveness??
  for (int i = 0;; i++) {
    qApp->processEvents();
    if (qthr.wait(100)) {
      break;
    }
    if (i == 20)
      progress.show();
    qApp->processEvents();
    if (progress.wasCanceled()) {
      // Just get out of there asap.
      exit(1);
    }

    qApp->processEvents();
  }

  int cnt = qthr.cnt;
  QString msg;
  if (cnt > 0) {
    QString str;
    msg = tr("Result count (est.)") + ": " + str.setNum(cnt);
  } else {
    msg = tr("No results found");
  }

  QApplication::restoreOverrideCursor();
  m_queryActive = false;
  restable->setEnabled(true);
  emit(resultsReady());
}

Widget::Widget(QWidget *parent) : QWidget(parent) {
  this->restable = new ResTable(this);
  this->searchLine = new SSearch();
  init_ui();
  init_conn();
}

Widget::~Widget() {}

void Widget::init_ui() {

  auto mvLayout = new QVBoxLayout();
  this->setLayout(mvLayout);
  mvLayout->addWidget(searchLine);
  auto mhLayout = new QHBoxLayout();
  mhLayout->addWidget(this->restable);
  mvLayout->addLayout(mhLayout);
}

void Widget::init_conn() {
  connect(this->searchLine, &SSearch::startSearch, this, &Widget::startSearch);
  //    restable->setRclMain(this, true);
  connect(this, SIGNAL(docSourceChanged(std::shared_ptr<DocSequence>)),
          restable, SLOT(setDocSource(std::shared_ptr<DocSequence>)));
  connect(this, SIGNAL(searchReset()), restable, SLOT(resetSource()));
  connect(this, SIGNAL(resultsReady()), restable, SLOT(readDocSource()));
  //    connect(this, SIGNAL(sortDataChanged(DocSeqSortSpec)),
  //        restable, SLOT(onSortDataChanged(DocSeqSortSpec)));
}
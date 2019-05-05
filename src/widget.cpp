#include "widget.h"
#include "ui_widget.h"

#include <QDebug>
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
//      LOGDEB("RclMain::startSearch. Indexing " << (m_idxproc?"on":"off") <<
//             " Active " << m_queryActive << "\n");
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
  if (!maybeOpenDb(reason, m_indexed, &b)) {
    m_indexed = false;
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
  dsss.field="relevancyrating";
  DocSeqFiltSpec dsfs;
//  dsfs.orCrit(DocSeqFiltSpec::DSFS_MIMETYPE,"application/x-all");
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

void Widget::IndexSomeFiles(QStringList paths) {
  QMutexLocker locker(&mtxTobeIndex);
  auto t = QSet<QString>::fromList(paths);
  tobeIndex.unite(t);
}

class IndexWorker : public QObject {
  Q_OBJECT
public:
  IndexWorker(QSet<QString> &origin, QMutex &m, QObject *parent)
      : QObject(parent), tobtIndex(origin), mutex(m) {}
public slots:
  void startIndex() {
    QSet<QString> s;
    {
      QMutexLocker locker(&mutex);
      s.unite(tobtIndex);
      tobtIndex.clear();
    }
    // TODO index
  }

private:
  QSet<QString> &tobtIndex;
  QMutex &mutex;
};
Widget::Widget(QWidget *parent) :DMainWindow(parent) {
  this->restable = new ResTable(this);
  this->searchLine = new SSearch(this);
  this->idxProcess = new QProcess(this);
  //    this->idxWorkerThread=new QThread(this);
  //    this->worker=new IndexWorker(this->tobeIndex,this->mtxTobeIndex,this);
  this->idxTimer = new QTimer;
  this->m_indexAvtive = false;
    this->m_queryActive=false;
  this->m_indexed = false;
  init_ui();
  init_conn();
  //  this->worker->moveToThread(this->idxWorkerThread);
  // TODO configure
  this->idxTimer->setInterval(20000);
}

Widget::~Widget() {}

void Widget::init_ui() {

  auto cw=new QWidget(this);
  this->setCentralWidget(cw);

  auto mvLayout = new QVBoxLayout();
  cw->setLayout(mvLayout);

  this->centralWidget()->setLayout(mvLayout);
  mvLayout->addWidget(searchLine);
  mvLayout->addWidget(restable);
  //  auto mhLayout = new QHBoxLayout();
  //  mhLayout->addWidget(this->restable);
  //  mvLayout->addLayout(mhLayout);
}

void Widget::init_conn() {
  connect(this->searchLine, &SSearch::startSearch, this, &Widget::startSearch);
  connect(this->searchLine,&SSearch::tabPressed,this->restable,&ResTable::moveToNextResoule);
  connect(this->searchLine,&SSearch::clearSearch,this->restable,&ResTable::clearSeach);
  connect(this->searchLine,&SSearch::returnPressed,this->restable,&ResTable::returnPressed);
  //    restable->setRclMain(this, true);
  connect(this, SIGNAL(docSourceChanged(std::shared_ptr<DocSequence>)),
          restable, SLOT(setDocSource(std::shared_ptr<DocSequence>)));
  connect(this, SIGNAL(searchReset()), restable, SLOT(resetSource()));
  connect(this, SIGNAL(resultsReady()), restable, SLOT(readDocSource()));
  connect(this->idxTimer, &QTimer::timeout, this, &Widget::toggleIndexing);
  connect(this->idxProcess,
          static_cast<void (QProcess::*)(int)>(&QProcess::finished), [this]() {
            this->m_indexAvtive = false;
            this->m_indexed = true;
          });
  connect(this->idxProcess, &QProcess::started,
          [this]() { this->m_indexAvtive = true; });

}

bool Widget::checkIdxPaths() {
  string badpaths;
  vector<string> args{"recollindex", "-c", theconfig->getConfDir(), "-E"};
  ExecCmd::backtick(args, badpaths);
  if (!badpaths.empty()) {
    int rep = QMessageBox::warning(0, tr("Bad paths"),
                                   tr("Bad paths in configuration file:\n") +
                                       QString::fromLocal8Bit(badpaths.c_str()),
                                   QMessageBox::Ok, QMessageBox::Cancel,
                                   QMessageBox::NoButton);
    if (rep == QMessageBox::Cancel)
      return false;
  }
  return true;
}

// This gets called when the "update index" action is activated. It executes
// the requested action, and disables the menu entry. This will be
// re-enabled by the indexing status check
void Widget::toggleIndexing() {
  if (m_indexAvtive) {
    qDebug() << "already indexing";
    return;
  }
  qDebug() << "start Index";
  QStringList sl;
  {
    QMutexLocker locker(&mtxTobeIndex);
    sl = QStringList::fromSet(tobeIndex);
    tobeIndex.clear();
  }

  QStringList args;
  args << " -c " + QString::fromStdString(theconfig->getConfDir());
  args << " -i " + sl.join(" ");

  idxProcess->start("recollindex", args);
}

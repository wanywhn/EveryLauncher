#include <utility>
#include <QDebug>
#include <QMessageBox>
#include <QProgressDialog>
#include <QThread>
#include <QVBoxLayout>
#include <docseqdb.h>
#include <wasatorcl.h>

#include "widget.h"
#include "ui_widget.h"


extern bool maybeOpenDb(string &reason, bool force, bool *maindberror);

// Start a db query and set the reslist docsource
void MainWindow::startSearch(string userInput) {


    string stemlang = "english";

    std::string reason;
    auto sdata = wasaStringToRcl(theconfig, stemlang,userInput, reason);

    std::shared_ptr<Rcl::SearchData> rsdata(sdata);

    if (m_queryActive) {
        qDebug()<<"startSearch already active";
        return;
    }
    m_queryActive = true;
    restable->setEnabled(false);
    m_source = std::shared_ptr<DocSequence>();

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


    auto *query = new Rcl::Query(rcldb.get());
    query->setCollapseDuplicates(true);

    DocSequenceDb *src =
            new DocSequenceDb(/*rcldb,*/ std::shared_ptr<Rcl::Query>(query),
                                         string(tr("Query results").toUtf8()), std::move(rsdata));
    src->setAbstractParams(true, false);
    m_source = std::shared_ptr<DocSequence>(src);

    DocSeqSortSpec dsss;
    dsss.field="mtype";
    DocSeqFiltSpec dsfs;

    m_source->setSortSpec(dsss);
    m_source->setFiltSpec(dsfs);

//  emit useFilterProxy();
    emit docSourceChanged(m_source);
    initiateQuery();
}

class QueryThread : public QThread {
    std::shared_ptr<DocSequence> m_source;

public:
    explicit QueryThread(std::shared_ptr<DocSequence> source) : m_source(std::move(source)) {}
    ~QueryThread() override = default;

    void run() override { cnt = m_source->getResCnt(); }
    int cnt{0};
};

void MainWindow::initiateQuery() {
    if (!m_source)
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QueryThread qthr(m_source);
    qthr.start();

//  DCircleProgress circleProgress(this);
//  circleProgress.setText(tr("正在进行查询，<bt>"
//                            "如果取消将退出程序。"));
//  circleProgress.setWindowModality(Qt::WindowModal);
    QProgressDialog progress(this);
    progress.setLabelText(tr("正在进行查询<br>"
                             "取消将退出程序<br>"));
    progress.setWindowModality(Qt::WindowModal);
    progress.setRange(0, 0);

    for (int i = 0;; i++) {
        qApp->processEvents();
        if (qthr.wait(100)) {
            break;
        }
        if (i == 20)
            progress.show();
//        circleProgress.show();
        qApp->processEvents();
        if (progress.wasCanceled()) {
            exit(1);
        }

        qApp->processEvents();
    }

    int cnt = qthr.cnt;
    qDebug()<<"count :"<<cnt;

    QApplication::restoreOverrideCursor();
    m_queryActive = false;
    restable->setEnabled(true);
    emit(resultsReady());
}

void MainWindow::IndexSomeFiles(QStringList paths) {
    auto t = QSet<QString>::fromList(paths);
    QMutexLocker locker(&mtxTobeIndex);
    tobeIndex.unite(t);
}

void MainWindow::filterChanged(QString field)
{
    DocSeqFiltSpec dsfs;
    dsfs.orCrit(DocSeqFiltSpec::DSFS_MIMETYPE,field.toStdString());
    m_source->setFiltSpec(dsfs);
    emit docSourceChanged(m_source);
    initiateQuery();

}

MainWindow::MainWindow(QWidget *parent) :DMainWindow(parent) {
    this->restable = new ResTable(this);
    this->searchLine = new SearchWidget(this);
    this->idxProcess = new QProcess(this);
    this->idxTimer = new QTimer(this);
    this->m_indexAvtive = false;
    this->m_queryActive=false;
    this->m_indexed = false;
    this->escKey=new QShortcut(QKeySequence(Qt::Key_Escape),this);
    this->upKey=new QShortcut(QKeySequence(Qt::Key_Up),this);
    this->downkey=new QShortcut(QKeySequence(Qt::Key_Down),this);
    init_ui();
    init_conn();
    // TODO configure
    this->idxTimer->setInterval(4000);
    this->idxTimer->start();
}


void MainWindow::init_ui() {

    auto cw=new QWidget(this);
    this->setCentralWidget(cw);

    auto mvLayout = new QVBoxLayout();
    cw->setLayout(mvLayout);

    this->centralWidget()->setLayout(mvLayout);
    mvLayout->addWidget(searchLine);
    mvLayout->addWidget(restable);
}

void MainWindow::init_conn() {
    connect(this->searchLine, &SearchWidget::startSearch, this, &MainWindow::startSearch);
    connect(this->searchLine,&SearchWidget::clearSearch,this->restable,&ResTable::clearSeach);

//  connect(this->searchLine,&SearchWidget::tabPressed,this->restable,&ResTable::moveToNextResoule);
    connect(this->searchLine,&SearchWidget::returnPressed,this->restable,&ResTable::returnPressed);

    connect(restable,&ResTable::filterChanged,this,&MainWindow::filterChanged);

    connect(this, SIGNAL(docSourceChanged(std::shared_ptr<DocSequence>)),
            restable, SLOT(setDocSource(std::shared_ptr<DocSequence>)));
    connect(this, SIGNAL(searchReset()), restable, SLOT(resetSource()));
    connect(this, SIGNAL(resultsReady()), restable, SLOT(readDocSource()));
    connect(this,&MainWindow::useFilterProxy,restable,&ResTable::useFilterProxy);


    connect(this->escKey,&QShortcut::activated,this->searchLine,&SearchWidget::clearAll);
    connect(this->upKey,&QShortcut::activated,this->restable,&ResTable::currentMoveUp);
    connect(this->downkey,&QShortcut::activated,this->restable,&ResTable::currentMoveDown);

    connect(this->idxTimer, &QTimer::timeout, this, &MainWindow::toggleIndexing);

    connect(this->idxProcess,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished)
            , [this]() {
                qDebug()<<"fi1";
                this->m_indexAvtive = false;
                this->m_indexed = true;
            });
    connect(this->idxProcess,&QProcess::errorOccurred,[this](){
        qDebug()<<"fi2";
        this->m_indexAvtive = false;
        this->m_indexed = true;

    });
    connect(this->idxProcess, &QProcess::started,
            [this]() { this->m_indexAvtive = true; });

}

void MainWindow::toggleIndexing() {
    if (m_indexAvtive) {
        qDebug() << "already indexing";
        return;
    }
    QStringList sl;
    {
        QMutexLocker locker(&mtxTobeIndex);
        sl = QStringList::fromSet(tobeIndex);
        tobeIndex.clear();
    }

    if(sl.size()==0){
        return;
    }
    QStringList args;
    args << "-c";
    args<<QString::fromStdString(theconfig->getConfDir());
    args << "-i";
    for(auto item:sl){
        args<<item;
    }

    idxProcess->start("recollindex", args);
}

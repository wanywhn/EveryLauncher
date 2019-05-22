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


void MainWindow::IndexSomeFiles(QStringList paths) {
    auto t = QSet<QString>::fromList(paths);
    QMutexLocker locker(&mtxTobeIndex);
    tobeIndex.unite(t);
}

//void MainWindow::filterChanged(QString field)
//{
//    DocSeqFiltSpec dsfs;
//    dsfs.orCrit(DocSeqFiltSpec::DSFS_MIMETYPE,field.toStdString());
//    m_source->setFiltSpec(dsfs);
//    emit docSourceChanged(m_source);
//    initiateQuery();
//
//}

MainWindow::MainWindow(QWidget *parent) :DMainWindow(parent) {
    this->restable = new ResWidget(this);
    this->searchLine = new SearchWidget(this);
    this->idxProcess = new QProcess(this);
    this->idxTimer = new QTimer(this);
    this->m_model=new UnitedModel(this);
    this->m_indexAvtive = false;
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

    restable->setM_model(this->m_model);
}

void MainWindow::init_conn() {
    connect(this->searchLine, &SearchWidget::startSearch,m_model, &UnitedModel::startSearch);
//    connect(this->searchLine,&SearchWidget::clearSearch,this->restable,&ResWidget::clearSeach);

//  connect(this->searchLine,&SearchWidget::tabPressed,this->restable,&ResWidget::moveToNextResoule);
    connect(this->searchLine,&SearchWidget::returnPressed,this->restable,&ResWidget::returnPressed);

//    connect(restable,&ResWidget::filterChanged,this,&MainWindow::filterChanged);

//    connect(this,&MainWindow::useFilterProxy,restable,&ResWidget::useFilterProxy);


    connect(this->escKey,&QShortcut::activated,this->searchLine,&SearchWidget::clearAll);
    connect(this->upKey,&QShortcut::activated,this->restable,&ResWidget::currentMoveUp);
    connect(this->downkey,&QShortcut::activated,this->restable,&ResWidget::currentMoveDown);

    connect(this->idxTimer, &QTimer::timeout, this, &MainWindow::toggleIndexing);

    connect(this->idxProcess,QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished)
            , [this]() {
                qDebug()<<"fi1";
                this->m_indexAvtive = false;
//                this->m_indexed = true;
            });
    connect(this->idxProcess,&QProcess::errorOccurred,[this](){
        qDebug()<<"fi2";
        this->m_indexAvtive = false;
//        this->m_indexed = true;

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

//
// Created by tender on 19-5-24.
//

#include <QtCore/QMutexLocker>
#include "IndexModule.h"
#include "config.h"
#include "dbusproxy.h"
#include <QDebug>

void IndexModule::IndexSomeFiles(QStringList paths) {
    auto t = QSet<QString>::fromList(paths);
    QMutexLocker locker(&mtxTobeIndex);
    tobeIndex.unite(t);
}
void IndexModule::toggleIndexing() {
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

    if(sl.empty()){
        return;
    }
    QStringList args;
    args << "-c";
    args<<QString::fromStdString(theconfig->getConfDir());
    args << "-i";
    for(const auto &item:sl){
        args<<item;
    }

    qDebug()<<"index args:"<<args;
    idxProcess->start("recollindex", args);
    emit sourceChanged();
}

IndexModule::IndexModule() {
    this->idxProcess = new QProcess(this);
    this->m_indexAvtive = false;
    this->idxTimer = new QTimer(this);
    connect(this->idxProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [this]() {
        qDebug() << "fi1";
        this->m_indexAvtive = false;
    });
    connect(this->idxProcess, &QProcess::errorOccurred, [this]() {
        qDebug() << "fi2";
        this->m_indexAvtive = false;

    });
    connect(this->idxProcess, &QProcess::started,
            [this]() { this->m_indexAvtive = true; });
//        m_model=m;
//    }

    connect(this->idxTimer, &QTimer::timeout, this, &IndexModule::toggleIndexing);
    // TODO configure
    this->idxTimer->setInterval(4000);
    this->idxTimer->start();
    connect(DBusProxy::getInstance(),&DBusProxy::mIndexChangeFiles,this,&IndexModule::IndexSomeFiles);
}

//
// Created by tender on 19-5-24.
//

#ifndef EVERYLAUNCHER_INDEXMODULE_H
#define EVERYLAUNCHER_INDEXMODULE_H


#include <QtCore/QTimer>
#include <QtCore/QSet>
#include <QtCore/QProcess>
#include <QMutex>

class IndexModule: public QObject {
    Q_OBJECT

public:
    IndexModule();
public slots:
    void IndexSomeFiles(QStringList paths);
signals:
    void sourceChanged();

private:
    virtual void toggleIndexing();
    QTimer *idxTimer;
    QSet<QString> tobeIndex;
    QMutex mtxTobeIndex;
    QProcess *idxProcess;
    bool m_indexAvtive;

};


#endif //EVERYLAUNCHER_INDEXMODULE_H

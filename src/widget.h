#ifndef WIDGET_H
#define WIDGET_H

#include "reslistwidget.h"
#include "searchline.h"

#include <QSet>
#include <QLabel>
#include <QMutex>
#include <QProcess>
#include <QTimer>
#include <QWidget>
#include <execmd.h>
#include <DtkWidgets>
#include <DMainWindow>

DWIDGET_USE_NAMESPACE
namespace Ui {
class Widget;
}
class IndexWorker;

class Widget : public DMainWindow
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    public slots:
virtual void startSearch(std::shared_ptr<Rcl::SearchData> sdata, bool issimple);
    virtual void initiateQuery();
    void IndexSomeFiles(QStringList paths);
signals:
    void resultsReady();
    void docSourceChanged(std::shared_ptr<DocSequence>);
    void searchReset();
    void useFilterProxy();
public slots:
    void filterChanged(QString field);
//private slots:
//    void pfinished(int );
private:
    void init_ui();
    void init_conn();
    virtual bool checkIdxPaths();
    virtual void toggleIndexing();
private:
    QThread *idxWorkerThread;
    IndexWorker *worker;
    QTimer	*idxTimer;

    std::shared_ptr<DocSequence> m_source;
    ResTable *restable;
    SSearch *searchLine;
    QSet<QString> tobeIndex;
    QMutex mtxTobeIndex;
    QProcess *idxProcess;
    bool m_queryActive;
    bool m_indexAvtive;
    bool m_indexed;
};

#endif // WIDGET_H

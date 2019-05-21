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
#include <QShortcut>

DWIDGET_USE_NAMESPACE
namespace Ui {
class Widget;
}
class IndexWorker;

class MainWindow : public DMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

public slots:
virtual void startSearch(string userInput);
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

    virtual void toggleIndexing();
private:
    QTimer	*idxTimer;

    std::shared_ptr<DocSequence> m_source;
    ResTable *restable;
    SearchWidget *searchLine;
    QSet<QString> tobeIndex;
    QMutex mtxTobeIndex;
    QProcess *idxProcess;
    bool m_queryActive;
    bool m_indexAvtive;
    bool m_indexed;
    QShortcut *escKey;
    QShortcut *upKey;
    QShortcut *downkey;
};

#endif // WIDGET_H

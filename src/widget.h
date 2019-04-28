#ifndef WIDGET_H
#define WIDGET_H

#include "reslistwidget.h"
#include "searchline.h"

#include <QWidget>
#include <execmd.h>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    public slots:
virtual void startSearch(std::shared_ptr<Rcl::SearchData> sdata, bool issimple);
    virtual void initiateQuery();
signals:
    void resultsReady();
    void docSourceChanged(std::shared_ptr<DocSequence>);
    void searchReset();
private:
    void init_ui();
    void init_conn();
private:
    ExecCmd          *m_idxproc{0}; // Indexing process
    std::shared_ptr<DocSequence> m_source;
    ResTable *restable;
    SSearch *searchLine;
    bool m_queryActive;
};

#endif // WIDGET_H

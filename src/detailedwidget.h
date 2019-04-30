#ifndef DETAILEDWIDGET_H
#define DETAILEDWIDGET_H

#include <QModelIndex>
#include <QStackedWidget>
#include <QWidget>
#include <rcldoc.h>

class DetailedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    explicit DetailedWidget(QWidget *parent = nullptr);

    void showDocDetail(QModelIndex index,Rcl::Doc doc);
signals:

public slots:
private:
};

#endif // DETAILEDWIDGET_H

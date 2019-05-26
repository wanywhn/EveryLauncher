#ifndef FIRSTTIMEINIT_H
#define FIRSTTIMEINIT_H

#include <QObject>
#include <QPushButton>
#include <DtkWidgets>
#include <QStringListModel>
#include <QProgressDialog>

class firstTimeInit : public DDialog
{
    Q_OBJECT
public:
    explicit firstTimeInit(QWidget *parent);

private:
    void init_ui();
    void init_conn();
    void setTopdirsAndIndex();
private:
    DTextButton *plusBtn;
    DTextButton *minusBtn;
    DTextButton *startBtn;
    DListView *listview;
    QStringListModel *model;
  QStringList lists;
     QProgressDialog *cp;

};

#endif // FIRSTTIMEINIT_H

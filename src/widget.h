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

class MainWindow : public DMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override = default;

private:
    void init_ui();
    void init_conn();

private:
    ResWidget *restable;
    UnitedModel *m_model;
    SearchWidget *searchLine;
    QShortcut *escKey;
    QShortcut *upKey;
    QShortcut *downkey;
};

#endif // WIDGET_H

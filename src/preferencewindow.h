#ifndef PREFERENCEWINDOW_H
#define PREFERENCEWINDOW_H

#include "configlistwidget.h"

#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QObject>
#include <QPushButton>
#include <QSettings>
#include <QTabWidget>

class PreferenceWindow : public QDialog
{
public:
    PreferenceWindow(QWidget *parent);


private:
    QSettings *msetting;
    QTabWidget *tabWidget;
    QWidget	*tabGeneral;
    QWidget *tabIndex;
    QWidget *tabIndexSche;
    QPushButton *general_index_update;
    QPushButton *general_index_reindex;
    ConfigListWidget *topDir;
    ConfigListWidget *skipDir;

//    QCheckBox *ckb_show_window_on_start=QCheckBox
    QCheckBox *ckb_show_indicator;
    QDialogButtonBox *btnBox;

private:
    void init_ui();
    void init_conn();
    void read_settings();



};

#endif // PREFERENCEWINDOW_H

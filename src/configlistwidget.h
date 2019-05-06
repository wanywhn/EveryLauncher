#ifndef CONFIGLISTWIDGET_H
#define CONFIGLISTWIDGET_H

#include <QFile>
#include <QGroupBox>
#include <QListView>
#include <QObject>
#include <QPushButton>
#include <QSettings>
#include <QStringListModel>

class ConfigListWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit ConfigListWidget(QString showGroupName,QString section="DEFAULT",QWidget *parent=nullptr);

    static void initConfigFile();
private:
    QString mgroupName;
    QString msection;
    QPushButton *btnAdd;
    QPushButton *btnDelete;
    QListView *listView;
    QStringListModel *listmodel;
    static QMap<QString,QString> msettings;
    static QFile *confFile;
    static QTextStream *qts;


private:
    void init_ui();
    void init_data();
    void init_conn();
private slots:
    void btn_add();
    void btn_delete();
public slots:
    void save_config();
};

#endif // CONFIGLISTWIDGET_H

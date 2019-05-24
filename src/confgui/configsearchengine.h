#ifndef CONFIGSEARCHENGINE_H
#define CONFIGSEARCHENGINE_H

#include <QPushButton>
#include <QSettings>
#include <QTableWidget>
#include <QWidget>

class ConfigSearchEngine : public QWidget
{
    Q_OBJECT
public:
    explicit ConfigSearchEngine(QWidget *parent = nullptr);

signals:

private slots:
    void addRow();
    void deleteRow();
    void cellChanged(int r,int c);
private:
    QTableWidget *twidget;
    QSettings sett;
    QPushButton *btnAdd;
    QPushButton *btnDelete;
private:
    void init_ui();
    void init_data();
};

#endif // CONFIGSEARCHENGINE_H

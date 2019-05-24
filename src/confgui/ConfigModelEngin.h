//
// Created by tender on 19-5-24.
//

#ifndef EVERYLAUNCHER_CONFIGMODELENGIN_H
#define EVERYLAUNCHER_CONFIGMODELENGIN_H



#include <QPushButton>
#include <QSettings>
#include <QWidget>
#include <QTableView>

class ConfigModelEngine :public QWidget{
Q_OBJECT
public:
    explicit ConfigModelEngine(QWidget *parent = nullptr);


private:
    QTableView *twidget;
private:
    void init_ui();

};


#endif //EVERYLAUNCHER_CONFIGMODELENGIN_H

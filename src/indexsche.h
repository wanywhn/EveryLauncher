#ifndef INDEXSCHE_H
#define INDEXSCHE_H

#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

class IndexSche : public QWidget
{
    Q_OBJECT
public:
    explicit IndexSche(QWidget *parent = nullptr);

private:

    void init_ui();
    void init_conn();
private:
    QLineEdit *lbCronWeek;
    QLineEdit *lbCronHour;
    QLineEdit *lbCronMinu;
    QPushButton *btnEnable;
    QPushButton *btnDisable;
    QCheckBox	*ckbStartOnStart;
    QCheckBox	*ckbStartNew;

signals:

public slots:
};

#endif // INDEXSCHE_H

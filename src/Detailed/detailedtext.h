#ifndef DETAILEDTEXT_H
#define DETAILEDTEXT_H

#include <QTextEdit>
#include <QWidget>
#include <rcldoc.h>

class DetailedW : public QWidget
{
    Q_OBJECT
public:
    explicit DetailedW(QWidget *parent = nullptr);

    virtual void showDoc(Rcl::Doc doc);
signals:

public slots:
private:
};

#endif // DETAILEDTEXT_H

#ifndef DETAILEDTEXT_H
#define DETAILEDTEXT_H

#include <QModelIndex>
#include <QTextEdit>
#include <QWidget>
#include <hldata.h>
#include <rcldoc.h>

class DetailedW : public QWidget
{
    Q_OBJECT
public:
    explicit DetailedW(QWidget *parent = nullptr);

    virtual void showDoc(Rcl::Doc doc);
    void setHighlightData(HighlightData hl);
    void setIndex(const QModelIndex &value);
    //两种方法，一是子类在setSupportedMimeType中添加自己支持的mime，二是直接重写支个方法
    virtual bool supportMimeType(std::string &m);

    std::set<std::string> getSupportedMimeTypes(){return setSupportedMimeType;}

signals:

public slots:
protected:
    HighlightData m_hData;
    QModelIndex index;
    std::set<std::string> setSupportedMimeType;
};

#endif // DETAILEDTEXT_H

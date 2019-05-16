#ifndef DETAILEDTEXT_H
#define DETAILEDTEXT_H

#include <QModelIndex>
#include <QSet>
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
    bool supportMimeType(std::string &m);

signals:

public slots:
protected:
    HighlightData m_hData;
    QModelIndex index;
    QSet<std::string> setSupportedMimeType;
};

#endif // DETAILEDTEXT_H

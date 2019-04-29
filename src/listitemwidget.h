#ifndef LISTITEMWIDGET_H
#define LISTITEMWIDGET_H

#include <QLabel>
#include <QWidget>

class ListItemWidget : public QWidget
{
    //TODO height?
    Q_OBJECT
public:
    explicit ListItemWidget(QWidget *parent = nullptr);
    //TODO WHY???
    void setIcon(const QPixmap &p)const;
    void setTitle(const QString &t)const;

signals:

public slots:

private:
    void init_ui();

private:
//    QPixmap icon;
    QLabel *icon;
    QLabel *title;

};

#endif // LISTITEMWIDGET_H

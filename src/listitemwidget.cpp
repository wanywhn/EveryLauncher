#include "listitemwidget.h"

#include <QVBoxLayout>

ListItemWidget::ListItemWidget(QWidget *parent) : QWidget(parent)
{

    this->icon=new QLabel;
    this->title=new QLabel;
    init_ui();
}

void ListItemWidget::setIcon(const QPixmap &p) const
{
   this->icon->setPixmap(p) ;
}

void ListItemWidget::setTitle(const QString &t) const
{
    this->title->setText(t);
}

void ListItemWidget::init_ui()
{
    auto vlayout=new QVBoxLayout();
    this->setLayout(vlayout);
    vlayout->addWidget(this->icon);
    vlayout->addWidget(this->title);
}

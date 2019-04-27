#include "indexsche.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

IndexSche::IndexSche(QWidget *parent) : QWidget(parent)
{
    this->lbCronHour=new QLineEdit();
    this->lbCronMinu=new QLineEdit();
    this->lbCronWeek=new QLineEdit();
    this->btnDisable=new QPushButton(tr("Disable"));
    this->btnEnable=new QPushButton(tr("Enable"));
    this->ckbStartNew=new QCheckBox();
    this->ckbStartOnStart=new QCheckBox();

    init_ui();
    init_conn();
}

void IndexSche::init_ui()
{
    auto vlayout=new QVBoxLayout();
    this->setLayout(vlayout);
    auto groupCron=new QGroupBox(tr("Cron"));
    vlayout->addWidget(groupCron);
    auto formLayoutCron=new QFormLayout();
    groupCron->setLayout(formLayoutCron);

    formLayoutCron->addWidget(new QLabel("xxx"));
    formLayoutCron->addRow(tr("星期"),this->lbCronWeek);
    formLayoutCron->addRow(tr("小时"),this->lbCronHour);
    formLayoutCron->addRow(tr("分钟"),this->lbCronMinu);

    auto hlayout=new QHBoxLayout();
    hlayout->addWidget(this->btnEnable);
    hlayout->addWidget(this->btnDisable);
    formLayoutCron->addWidget(new QLabel("xxx"));
    formLayoutCron->addRow(hlayout);

    auto groupRealtim=new QGroupBox(tr("RealTime"));
    vlayout->addWidget(groupRealtim);

    auto formLayout=new QFormLayout();
    groupRealtim->setLayout(formLayout);
    formLayout->addWidget(new QLabel(tr("aaa")));


    this->ckbStartNew->setEnabled(false);
    formLayout->addRow(this->ckbStartOnStart,new QLabel("Start On System Start"));
    formLayout->addRow(this->ckbStartNew,new QLabel(tr("Start Now")));

}

void IndexSche::init_conn()
{
    connect(this->btnEnable,&QPushButton::clicked,[](){
        //TODO
    });
    connect(this->btnDisable,&QPushButton::clicked,[](){
        //TODO
    });
    connect(this->ckbStartNew,&QCheckBox::toggled,[](){

    });
    connect(this->ckbStartOnStart,&QCheckBox::toggled,[this](bool checked){
        if(checked){
            this->ckbStartNew->setEnabled(true);
        }

    });

}

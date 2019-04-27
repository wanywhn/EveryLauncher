#include "preferencewindow.h"
#include "config.h"
#include "indexsche.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QDebug>

PreferenceWindow::PreferenceWindow(QWidget *parent) : QDialog(parent) {
//    this->setAttribute(Qt::WA_DeleteOnClose);
  this->msetting = new QSettings(ORGANIZATION_NAME, AppName, parent);
  this->tabWidget = new QTabWidget(this);
  this->tabGeneral = new QWidget(this);
  this->tabIndex = new QWidget(this);
  this->tabIndexSche = new QWidget(this);
  this->ckb_show_indicator = new QCheckBox(this);
  this->general_index_update = new QPushButton(this->tr("Update Index"));
  this->general_index_reindex = new QPushButton(this->tr("Reindex"));
  this->btnBox =
      new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    init_ui();
    init_conn();
}

void PreferenceWindow::init_ui() {
  auto vmlayout = new QVBoxLayout();
  this->tabGeneral->setLayout(vmlayout);

  auto general_group_box = new QGroupBox(this->tr("General"));
  vmlayout->addWidget(general_group_box);


  auto general_left_formlayout = new QFormLayout();
  general_group_box->setLayout(general_left_formlayout);

  general_left_formlayout->addRow(this->tr("Show Indicator"),
                                  this->ckb_show_indicator);

  auto general_group_index = new QGroupBox(this->tr("Index"));
  vmlayout->addWidget(general_group_index);

  auto general_index_formlayout = new QFormLayout();
  general_group_index->setLayout(general_index_formlayout);

  general_index_formlayout->addWidget(this->general_index_update);
  general_index_formlayout->addWidget(this->general_index_reindex);

  auto index_vlayout = new QVBoxLayout();
  this->tabIndex->setLayout(index_vlayout);

  ConfigListWidget::initConfigFile();
  this->topDir = new ConfigListWidget("TopDir",
                                   "topdirs");
  this->skipDir =new
      ConfigListWidget( "SkipDir",  "skippedPaths");
          index_vlayout->addWidget(this->topDir);
  index_vlayout->addWidget(this->skipDir);

  this->tabIndexSche = new IndexSche();


  this->tabWidget->addTab(this->tabGeneral, this->tr("General"));
  this->tabWidget->addTab(this->tabIndex, this->tr("Global Index"));
  this->tabWidget->addTab(this->tabIndexSche, this->tr("Sched"));

  auto main_layout = new QVBoxLayout();
  main_layout->addWidget(this->tabWidget);
  main_layout->addWidget(this->btnBox);
  this->setLayout(main_layout);
}

void PreferenceWindow::init_conn()
{

    connect(this->btnBox,&QDialogButtonBox::accepted,[this](){
        qDebug()<<"accepted";
        this->topDir->save_config();
        this->skipDir->save_config();
        this->close();
    });
    connect(this->btnBox,&QDialogButtonBox::rejected,[this](){
        this->close();
    });
}

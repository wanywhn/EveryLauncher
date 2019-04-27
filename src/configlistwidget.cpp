#include "configlistwidget.h"
#include "config.h"

#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>
#include <QVBoxLayout>
#include <QDebug>
#include <QProcess>
#include <QFileDialog>

ConfigListWidget::ConfigListWidget(QString showGroupName, QString section,
                                   QWidget *parent)
    :QGroupBox (showGroupName,parent) ,mgroupName(showGroupName), msection(section) {
  // TODO conf Parser
  btnAdd = new QPushButton(tr("Add"));
  btnDelete = new QPushButton(tr("Delete"));
  listView = new QListView(this);
  listmodel=new QStringListModel(this);

  init_ui();
  init_data();
  init_conn();
}

void ConfigListWidget::initConfigFile()
{
  auto filedir =QDir(
      QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
  filedir.cd(RECOLL_CONFIG_DIR);
  auto filepath = filedir.absoluteFilePath(RECOLL_CONFIG_FILE);
  if(confFile!=nullptr){
      confFile->close();
      delete confFile;
      confFile=nullptr;
  }
  confFile = new QFile(filepath);

  msettings.clear();
  if (confFile->exists()) {
      confFile->open(QFile::ReadWrite);
      if(qts==nullptr)
        qts=new QTextStream(confFile);
      while (!qts->atEnd()) {
          auto line=qts->readLine();
          while(line.endsWith('\\')){
//              line.replace('\\',' ');
              line+=qts->readLine();
          }
          auto tmp=line.trimmed().split("=");
          if(tmp.length()!=2){
              continue;
          }
          if(tmp[1].trimmed().isEmpty()){
              continue;
          }
          msettings[tmp[0]]=tmp[1];
//          auto k=tmp[0];
//          auto v=tmp[1];
      }
//   confFile->reset();

      if(!confFile->resize(0)){
          qDebug()<<"resize failed";
      }
      confFile->flush();
      qts->seek(0);

  }
}

void ConfigListWidget::init_ui() {
  auto vlayout = new QVBoxLayout();
  this->setLayout(vlayout);
  auto hlayout = new QHBoxLayout();

  hlayout->addWidget(this->btnAdd);
  hlayout->addWidget(this->btnDelete);

  vlayout->addWidget(this->listView);
  vlayout->addLayout(hlayout);
}

void ConfigListWidget::init_data()
{
//    qDebug()<<this->confgSettings->allKeys();
    auto items=msettings[msection].split("\\").replaceInStrings("\t","");
    this->listmodel->setStringList(items);
    this->listView->setModel(this->listmodel);

}

void ConfigListWidget::init_conn() {
    connect(this->btnAdd, &QPushButton::clicked, this,&ConfigListWidget::btn_add);
    connect(this->btnDelete, &QPushButton::clicked, this,&ConfigListWidget::btn_delete);
}

void ConfigListWidget::btn_add()
{

    auto path=QFileDialog::getExistingDirectory(this,tr("Add Monitor Floder"),QDir::homePath());
    if(this->listmodel->insertRow(this->listmodel->rowCount())){
        auto index=this->listmodel->index(this->listmodel->rowCount()-1,0);
        this->listmodel->setData(index,path);

    }
}
void ConfigListWidget::btn_delete()
{
    auto selectmodel=this->listView->selectionModel();
    for(auto item:selectmodel->selectedRows()){
        listmodel->removeRow(item.row());

    }



}

void ConfigListWidget::save_config()
{
   auto lm=this->listmodel->stringList();
//   qts->setDevice(confFile);
   *qts<<"\n";
   QString line;
   for(auto item:lm){
       if(item.trimmed().isEmpty())
           continue;
       line+='\t'+item+"\\\n";
   }
   line.replace(line.length()-2,2,' ');
//   ts.seek(0);
   *qts<<this->msection<<"="<<line;
   qts->flush();

}
QTextStream *ConfigListWidget::qts=nullptr;

QFile *ConfigListWidget::confFile=nullptr;
QMap<QString,QString> ConfigListWidget::msettings;

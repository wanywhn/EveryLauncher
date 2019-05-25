#include "desktoppreview.h"

#include <QDebug>
#include <QFile>
#include <QIcon>
#include <QSettings>
#include <QStringList>
#include <QVBoxLayout>
#include <Model/recollmodel.h>
DesktopPreview::DesktopPreview() {
        setSupportedMimeType=std::set<std::string>({
//    setSupportedMimeType.fromList({

             "application/x-all",

                                  });
  icon = new QLabel(this);
  appName = new QLabel(this);
  comment=new QLabel(this);
  appInstalledTime=new QLabel(this);
  appSize=new QLabel(this);
  appVersion=new QLabel(this);
  init_ui();
}

void DesktopPreview::showDoc() {

//  auto path = index.data(ELModelInterface::Role_LOCATION).toString();
//  auto pixmap=QPixmap(index.data(ELModelInterface::Role_ICON_ByteArray).toString());
//  icon->setPixmap(pixmap.scaled(128,128));
  auto pixmap=index.data(ELModelInterface::Role_ICON_ByteArray).value<QIcon>();
  icon->setPixmap(pixmap.pixmap(192));
//  icon->setPixmap(pixmap);
  //TODO change size
  appName->setText(index.data(ELModelInterface::Role_APP_NAME).toString());
  comment->setText(index.data(ELModelInterface::Role_APP_COMMENT).toString());
  appVersion->setText(index.data(ELModelInterface::Role_APP_VERSION).toString());

  auto locale=this->locale();
  //TODO wneh index desktop file ,wo can collect app info,and display here
}


void DesktopPreview::init_ui() {
  auto vlayout = new QVBoxLayout();
  this->setLayout(vlayout);
  this->comment->setWordWrap(true);

  auto iconhLayout=new QHBoxLayout();
  iconhLayout->addStretch(0);

  auto vtopLayout=new QVBoxLayout();
  auto h=new QHBoxLayout();
  h->addStretch(0);
  h->addWidget(icon);
  h->addStretch(0);
  vtopLayout->addLayout(h);


  h=new QHBoxLayout();
  h->addStretch(0);
  h->addWidget(appName);
  h->addStretch(0);
  vtopLayout->addLayout(h);

  h=new QHBoxLayout();
  h->addStretch(0);
  h->addWidget(appVersion);
  h->addStretch(0);
  vtopLayout->addLayout(h);

  h=new QHBoxLayout();
  h->addStretch(0);
  h->addWidget(comment);
  h->addStretch(0);
  vtopLayout->addLayout(h);

  iconhLayout->addLayout(vtopLayout);

  iconhLayout->addStretch(0);
  vlayout->addLayout(iconhLayout);


  auto apitLayout=new QHBoxLayout();
  apitLayout->addStretch(0);
  apitLayout->addWidget(appInstalledTime);
  apitLayout->addStretch(0);
  vlayout->addLayout(apitLayout);

  auto apszLayout=new QHBoxLayout();
  apszLayout->addStretch(0);
  apszLayout->addWidget(appSize);
  apszLayout->addStretch(0);
  vlayout->addLayout(apszLayout);

}

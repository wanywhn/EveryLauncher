#include "desktoppreview.h"

#include <QDebug>
#include <QFile>
#include <QIcon>
#include <QSettings>
#include <QStringList>
#include <QVBoxLayout>
#include <recollmodel.h>

DesktopPreview::DesktopPreview() {
  icon = new QLabel(this);
  appName = new QLabel(this);
  comment=new QLabel(this);
  init_ui();
}

void DesktopPreview::showDoc(Rcl::Doc doc) {

  auto path = index.data(RecollModel::Role_LOCATION).toString();
  auto pixmap=QPixmap(index.data(RecollModel::Role_ICON_PATH).toString());
  icon->setPixmap(pixmap);
  appName->setText(index.data(RecollModel::Role_APP_NAME).toString());
  comment->setText(index.data(RecollModel::Role_APP_COMMENT).toString());

qDebug() << "path:" << path;
}

void DesktopPreview::init_ui() {
  auto vlayout = new QVBoxLayout();
  this->setLayout(vlayout);
  vlayout->addWidget(icon);
  vlayout->addWidget(appName);
  vlayout->addWidget(comment);
}

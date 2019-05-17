#include "detailedwidget.h"
#include "reslistwidget.h"
#include <Detailed/desktoppreview.h>
#include <Detailed/imagepreview.h>
#include <Detailed/pdfpreview.h>
#include <Detailed/player/player.h>
#include <Detailed/preview_w.h>
#include <QDebug>
#include <QMediaPlayer>

//    QStringList supportedMimeTypes = m_player->supportedMimeTypes();
//    if (!supportedMimeTypes.isEmpty()) {
//        supportedMimeTypes.append("audio/x-m3u"); // MP3 playlists
// static QMap<QString,int> str2idx({
//             {"default",0},
//                                 });
static QMap<std::string, DetailedW *> wididx;
DetailedWidget::DetailedWidget(QWidget *parent) : QStackedWidget(parent) {
  // the default widget show common info
  addWidget( new DetailedW);
  addWidget( new Preview);
  addWidget( new DesktopPreview);
  addWidget( new PdfPreview(this));
  addWidget(new imagePreview(this));
  addWidget(new Player(this));
  for (int i = 0; i != this->count(); ++i) {
    auto w = qobject_cast<DetailedW *>(this->widget(i));
    for (auto m : w->getSupportedMimeTypes()) {
      wididx.insert(m, w);
    }
  }
}

void DetailedWidget::showDocDetail(QModelIndex index, Rcl::Doc doc,
                                   HighlightData hl) {
      qDebug()<<"doc mime"<<QString::fromStdString(doc.mimetype)<<"path:"<<QString::fromStdString(doc.url);
//      qDebug()<<"item
  //    mime:"<<index.data(RecollModel::ModelRoles::Role_MIME_TYPE).toString();
  //   auto
  //   wid=str2idx[index.data(RecollModel::ModelRoles::Role_MIME_TYPE).toString()];
  auto wid = wididx.value(doc.mimetype);
  if (wid == nullptr) {
      wid=qobject_cast<DetailedW *>(this->widget(0));
  }
  this->setCurrentWidget(wid);
  //   auto curr=qobject_cast<DetailedW *>( this->currentWidget());
  wid->setHighlightData(hl);
  wid->setIndex(index);
  wid->showDoc(doc);
}

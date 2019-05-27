#include "detailedwidget.h"
#include "reslistwidget.h"
#include "Detailed/desktoppreview.h"
#include "Detailed/imagepreview.h"
#include "Detailed/pdfpreview.h"
#include "Detailed/player/player.h"
#include "Detailed/text_preview.h"
#include <QDebug>
#include <QMediaPlayer>
#include <Detailed/map_preview.h>
#include <Detailed/WeatherPreview.h>


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
  addWidget(new map_preview);
//  addWidget(new WeatherPreview);
  for (int i = 0; i != this->count(); ++i) {
    auto w = qobject_cast<DetailedW *>(this->widget(i));
    for (const auto &m : w->getSupportedMimeTypes()) {
      wididx.insert(m, w);
    }
  }
}

void DetailedWidget::showDocDetail(QModelIndex index) {
  auto mimetype=index.data(ELModelInterface::ModelRoles::Role_MIME_TYPE).toString().toStdString();
  auto wid = wididx.value(mimetype);
  if (wid == nullptr) {
      wid=qobject_cast<DetailedW *>(this->widget(0));
  }
  this->setCurrentWidget(wid);
  wid->setIndex(index);
  wid->showDoc();
}

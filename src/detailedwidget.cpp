#include "detailedwidget.h"
#include "reslistwidget.h"
#include <QDebug>
#include <QMediaPlayer>
#include <Detailed/desktoppreview.h>
#include <Detailed/imagepreview.h>
#include <Detailed/pdfpreview.h>
#include <Detailed/preview_w.h>

//    QStringList supportedMimeTypes = m_player->supportedMimeTypes();
//    if (!supportedMimeTypes.isEmpty()) {
//        supportedMimeTypes.append("audio/x-m3u"); // MP3 playlists
//static QMap<QString,int> str2idx({
//             {"default",0},
//                                 });
static QMap<std::string,DetailedW*> wididx;
DetailedWidget::DetailedWidget(QWidget *parent) :QStackedWidget(parent)
{
    //the default widget show common info
    insertWidget(0,new DetailedW);
    insertWidget(1,new Preview);
    insertWidget(2,new DesktopPreview);
    insertWidget(3,new PdfPreview(this));
    insertWidget(4,new imagePreview(this));
    std::vector<std::string> cates;
    theconfig->getMimeCategories(cates);
    std::vector<std::string> mimetypes;
    for(auto mt:cates){
        theconfig->getMimeCatTypes(mt,mimetypes);
        for(auto mime:mimetypes){
            bool finded=false;
            for(int i=1;i!=this->count();++i){
                auto w=qobject_cast<DetailedW*> (this->widget(i));
                if(w->supportMimeType(mime)){
                    finded=true;
                    wididx.insert(mime,w);
                }
            }
            if(!finded){
                wididx.insert(mime,qobject_cast<DetailedW*>(this->widget(0)));
            }
        }

    }
}

void DetailedWidget::showDocDetail(QModelIndex index, Rcl::Doc doc, HighlightData hl)
{
    qDebug()<<"doc mime"<<QString::fromStdString(doc.mimetype)<<" path:"<<QString::fromStdString(doc.url);
    qDebug()<<"item mime:"<<index.data(RecollModel::ModelRoles::Role_MIME_TYPE).toString();
//   auto wid=str2idx[index.data(RecollModel::ModelRoles::Role_MIME_TYPE).toString()];
   auto wid=wididx.value(doc.mimetype);
//   this->setCurrentIndex(wid);
   if(wid==nullptr){
       //TODO unknow should give a default preview
       return ;
   }
   this->setCurrentWidget(wid);
//   auto curr=qobject_cast<DetailedW *>( this->currentWidget());
   wid->setHighlightData(hl);
   wid->setIndex(index);
   wid->showDoc(doc);

}

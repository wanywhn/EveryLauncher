#include "detailedwidget.h"
#include "reslistwidget.h"
#include <QDebug>
#include <Detailed/desktoppreview.h>
#include <Detailed/imagepreview.h>
#include <Detailed/pdfpreview.h>
#include <Detailed/preview_w.h>

static QMap<QString,int> str2idx({
             {"default",0},{"text/plain",1},
             {"application/x-all",2},
             {"application/pdf",3},
             {"image/jpeg",4},
                                { "image/bmp",4},
                                { "image/gif",4},
                                { "image/jpeg",4},
                                { "image/png",4},
                                { "image/x-portable-bitmap",4},
                                { "image/x-portable-graymap",4},
                                { "image/x-portable-pixmap",4},
                                { "image/x-xbitmap",4},
                                { "image/x-xpixmap",4},
//                                { "image/svg+xml",4},

                                 });
DetailedWidget::DetailedWidget(QWidget *parent) :QStackedWidget(parent)
{
    insertWidget(0,new DetailedW);
    insertWidget(1,new Preview);
    insertWidget(2,new DesktopPreview);
    insertWidget(3,new PdfPreview(this));
    insertWidget(4,new imagePreview(this));
}

void DetailedWidget::showDocDetail(QModelIndex index, Rcl::Doc doc, HighlightData hl)
{
    qDebug()<<"item mime:"<<index.data(RecollModel::ModelRoles::Role_MIME_TYPE).toString();
   auto wid=str2idx[index.data(RecollModel::ModelRoles::Role_MIME_TYPE).toString()];
   this->setCurrentIndex(wid);
   auto curr=qobject_cast<DetailedW *>( this->currentWidget());
   curr->setHighlightData(hl);
   curr->setIndex(index);
   curr->showDoc(doc);

}

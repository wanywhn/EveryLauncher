#include "detailedwidget.h"
#include "reslistwidget.h"
#include <QDebug>
#include <Detailed/desktoppreview.h>
#include <Detailed/preview_w.h>

static QMap<QString,int> str2idx({
             {"default",0},{"text/plain",1},
             {"application/x-all",2},



                                 });
DetailedWidget::DetailedWidget(QWidget *parent) :QStackedWidget(parent)
{
    insertWidget(0,new DetailedW);
    insertWidget(1,new Preview);
    insertWidget(2,new DesktopPreview);
}

void DetailedWidget::showDocDetail(QModelIndex index, Rcl::Doc doc, HighlightData hl)
{
   auto wid=str2idx[index.data(RecollModel::ModelRoles::Role_MIME_TYPE).toString()];
   this->setCurrentIndex(wid);
   auto curr=qobject_cast<DetailedW *>( this->currentWidget());
   curr->setHighlightData(hl);
   curr->setIndex(index);
   curr->showDoc(doc);

}

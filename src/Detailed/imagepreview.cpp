#include "imagepreview.h"

#include <QVBoxLayout>
#include <recollmodel.h>

imagePreview::imagePreview(QWidget *parent):DetailedW (parent)
{
    this->img=new QPixmap();
    this->imgView=new QLabel(this);
//   imgView->setFixedSize(this->sizeHint());
    init_ui();

}

void imagePreview::init_ui()
{
    auto vlayout=new QVBoxLayout();
    vlayout->addWidget(imgView);
    this->setLayout(vlayout);
}

void imagePreview::showDoc(Rcl::Doc doc)
{
   auto filepath=index.data(RecollModel::RecollModel::Role_LOCATION).toString().replace("file://","");
   img->load(filepath);
   imgView->setPixmap(*img);
   imgView->setScaledContents(true);

}

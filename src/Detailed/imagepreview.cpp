#include "imagepreview.h"

#include <QVBoxLayout>
#include <recollmodel.h>

imagePreview::imagePreview(QWidget *parent):DetailedW (parent)
{
    setSupportedMimeType.fromList({

                                "image/jpeg",
                                 "image/bmp",
                                 "image/gif",
                                 "image/jpeg",
                                 "image/png",
                                 "image/x-portable-bitmap",
                                 "image/x-portable-graymap",
                                 "image/x-portable-pixmap",
                                 "image/x-xbitmap",
                                 "image/x-xpixmap",
                                 "image/svg+xml",

                                  });
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

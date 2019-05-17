#include "imagepreview.h"

#include <QFileInfo>
#include <QFormLayout>
#include <QLocale>
#include <QVBoxLayout>
#include <recollmodel.h>
#include <QDateTime>

imagePreview::imagePreview(QWidget *parent):DetailedW (parent)
{
//    setSupportedMimeType.fromList({
        setSupportedMimeType=std::set<std::string>({

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
        this->imgType=new QLabel(this);
        this->imgSize=new QLabel(this);
        this->imgFileSize=new QLabel(this);
        this->timeAcccess=new QLabel();
        this->timeChange=new QLabel();
        this->timeModify=new QLabel();

//   imgView->setFixedSize(this->sizeHint());
    init_ui();

}

void imagePreview::init_ui()
{
    auto vlayout=new QVBoxLayout();

    auto hinfolayout=new QHBoxLayout();

    auto infoLayout=new QFormLayout();
    infoLayout->addRow(new QLabel("FileType:"),imgType);
    infoLayout->addRow(new QLabel("ImageSize:"),imgSize);
    infoLayout->addRow(new QLabel("FileSize:"),imgFileSize);

    auto timeLayout=new QFormLayout();
    timeLayout->addRow(new QLabel("最近访问:"),timeAcccess);
    timeLayout->addRow(new QLabel("Rec Modify:"),timeModify);
    timeLayout->addRow(new QLabel("Rec Change:"),timeChange);

    hinfolayout->addStretch(1);
    hinfolayout->addLayout(timeLayout);
    hinfolayout->addLayout(infoLayout);
    hinfolayout->addStretch(1);

    vlayout->addWidget(imgView);
    vlayout->addLayout(hinfolayout);
    this->setLayout(vlayout);
}

void imagePreview::showDoc(Rcl::Doc doc)
{
   auto filepath=index.data(RecollModel::RecollModel::Role_LOCATION).toString().replace("file://","");
   img->load(filepath);
   imgView->setPixmap(*img);
   imgView->setScaledContents(true);

   QFileInfo info(QUrl(QString::fromStdString(doc.url)).toLocalFile());
   imgType->setText(QString::fromStdString(doc.mimetype));
   auto ims=QImage(info.absoluteFilePath()).size();
   imgSize->setText(QString("%1X%2").arg(ims.width()).arg(ims.height()));
   QLocale locale = this->locale();
   QString valueText = locale.formattedDataSize(info.size());
   imgFileSize->setText(valueText);

   timeAcccess->setText(info.lastRead().toString());
   //TODO empty ?
   timeModify->setText(info.lastModified().toString());
   timeChange->setText(info.metadataChangeTime().toString());

}

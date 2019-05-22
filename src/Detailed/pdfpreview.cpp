#include "pdfpreview.h"
#include <Model/recollmodel.h>
#include <QVBoxLayout>

PdfPreview::PdfPreview(QWidget *parent):DetailedW (parent)
{
        setSupportedMimeType=std::set<std::string>({
//    setSupportedMimeType.fromList({

             "application/pdf",
                                  });

    this->pdfView=new QPdfView(this);
    this->pdfdoc=new QPdfDocument(this);
    pdfView->setPageMode(QPdfView::PageMode::MultiPage);
    pdfView->setZoomMode(QPdfView::ZoomMode::FitToWidth);
    pdfView->setDocument(pdfdoc);
    init_ui();
}

void PdfPreview::init_ui()
{
    auto vlay=new QVBoxLayout(this);
    this->setLayout(vlay);
    vlay->addWidget(pdfView);

}

void PdfPreview::showDoc()
{
    auto path=index.data(ELModelInterface::Role_LOCATION).toString();
    path.replace("file://","");
    pdfdoc->load(path);

}

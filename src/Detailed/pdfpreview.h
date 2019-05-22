#ifndef PDFPREVIEW_H
#define PDFPREVIEW_H

#include "detailedtext.h"

#include <QObject>
#include <QtPdfWidgets/QPdfView>
#include <QtPdf/QPdfDocument>

class PdfPreview :public DetailedW
{
    Q_OBJECT
public:
    PdfPreview(QWidget *parent);
    void init_ui();
private:
    QPdfView *pdfView ;
    QPdfDocument *pdfdoc;

    // DetailedW interface
public:
    void showDoc() override;
};

#endif // PDFPREVIEW_H

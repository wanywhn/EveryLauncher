#ifndef IMAGEPREVIEW_H
#define IMAGEPREVIEW_H

#include "detailedtext.h"

#include <QLabel>
#include <QObject>
#include <QPixmap>

class imagePreview : public DetailedW
{
public:
    imagePreview(QWidget *parent);

private:
    void init_ui();

    QPixmap *img;
    QLabel *imgView;


    // DetailedW interface
public:
    void showDoc(Rcl::Doc doc) override;
};

#endif // IMAGEPREVIEW_H

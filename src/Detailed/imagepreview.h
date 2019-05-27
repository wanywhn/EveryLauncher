#ifndef IMAGEPREVIEW_H
#define IMAGEPREVIEW_H

#include "detailedtext.h"

#include <QLabel>
#include <QObject>
#include <QPixmap>
#include <QtWidgets/QScrollArea>

class imagePreview : public DetailedW
{
public:
    imagePreview(QWidget *parent);

private:
    void init_ui();

    QPixmap *img;
    QLabel *imgView;
    QLabel *imgType;
    QLabel *imgSize;
    QLabel *imgFileSize;
    QLabel *timeAcccess;
    QLabel *timeModify;
    QLabel *timeChange;
    QScrollArea *sa;


    // DetailedW interface
public:
    void showDoc() override;
};

#endif // IMAGEPREVIEW_H

#ifndef _PREVIEW_W_H_INCLUDED_
#define _PREVIEW_W_H_INCLUDED_

#include <stdio.h>
#include <memory>
#include <QComboBox>

#include "detailedtext.h"

class QTabWidget;
class QLabel;
class QPushButton;
class QCheckBox;
class Preview;
class PlainToRichQtPreview;
class QUrl;

class Preview : public DetailedW {
  Q_OBJECT

public:
  Preview( );


private:

  void init();
public:
  void showDoc() override;

private:
//    QTextBrowser *editor;
QLabel *label;

};

#endif /* _PREVIEW_W_H_INCLUDED_ */

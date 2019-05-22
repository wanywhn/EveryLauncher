#include <list>
#include <utility>

#include <QEventLoop>
#include <QShortcut>
#include <QTimer>
#include <qevent.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmenu.h>
#include <qmessagebox.h>
#include <qprogressdialog.h>
#include <qpushbutton.h>
#include <qscrollbar.h>
#include <qtabwidget.h>
#include <qtextbrowser.h>
#include <qtextedit.h>
#include <qthread.h>
#include <qurl.h>
#include <qvariant.h>

#include "text_preview.h"
#include <QApplication>
#include <Model/ELModelInterface.h>
#include <rclconfig.h>

extern RclConfig *theconfig;

Preview::Preview()// Search terms etc. for highlighting
        : DetailedW(nullptr) {
    setSupportedMimeType = std::set<std::string>({
//        setSupportedMimeType.fromList({

                                                         "text/plain",
                                                         "application/msword",

                                                 });
    init();
}

void Preview::init() {
    label=new QLabel();

    auto previewLayout = new QVBoxLayout(this);
    previewLayout->addWidget(label);


    this->setLayout(previewLayout);
}

void Preview::showDoc() {
    label->setText(index.data(ELModelInterface::ModelRoles::Role_FILE_SIMPLE_CONTENT).toString());

    return;
}



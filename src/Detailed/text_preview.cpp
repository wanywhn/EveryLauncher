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
//                                                         "application/msword",

                                                 });
    init();
}

void Preview::init() {
//    label = new QLabel();
//    label->setWordWrap(true);
//    label->setTextFormat(Qt::TextFormat::RichText);

    browser=new QTextBrowser;
    browser->setAcceptRichText(true);
    auto previewLayout = new QVBoxLayout(this);
    previewLayout->addWidget(browser);


    this->setLayout(previewLayout);
}

void Preview::showDoc() {
    browser->clear();
    auto sl=index.data(ELModelInterface::ModelRoles::Role_FILE_FULLTEXT_COLORED).toStringList();
    for(auto item:sl){
        browser->append(item);
    }
//    this->browser.ap

}



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
//    label = new QLabel();
//    label->setWordWrap(true);
//    label->setTextFormat(Qt::TextFormat::RichText);

    browser=new QTextBrowser;
    browser->setAcceptRichText(true);
    auto previewLayout = new QVBoxLayout(this);
    previewLayout->addWidget(browser);


    this->setLayout(previewLayout);
}
static QSet<QString> rawset({
    "application/msword",

});
void Preview::showDoc() {
    browser->clear();
    QStringList sl;
    auto mtype=index.data(ELModelInterface::ModelRoles::Role_MIME_TYPE).toString();

    if(rawset.contains(mtype)){
        sl=index.data(ELModelInterface::ModelRoles::Role_FILE_FULLTEXT_COLORED_FROM_CACHED).toStringList();
    }else{
        sl=index.data(ELModelInterface::ModelRoles::Role_FILE_FULLTEXT_COLORED_FROM_RAW).toStringList();
    }
//    QString s;
//    for(auto i:sl){
//        s.append(i);
//        s.append("\n");
//    }
//    browser->setHtml(s);
    for(auto item:sl){
        browser->append(item.replace("\n","<br>"));
    }
//    this->browser.ap

}



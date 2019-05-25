#include <utility>

#include "reslistwidget.h"
#include "SearchItemDelegate.h"

#include <QDebug>
#include <QHeaderView>
#include <QMessageBox>
#include <QPainter>
#include <QProcess>
#include <QShortcut>
#include <QSizePolicy>
#include <QStyledItemDelegate>
#include <QTextDocument>
#include <QTimer>
#include <QVBoxLayout>


void ResWidget::init_conn() {
//    connect(this, &ResWidget::currentChanged, this, &ResWidget::onTableView_currentChanged);
}

ResWidget::ResWidget(QWidget *parent)
        : QWidget(parent), m_ismainres(true) {

    listview = new DListView(this);

    this->dtw = new DetailedWidget();
    QStringList fields;
    init_ui();
    init_conn();
}


void ResWidget::init_ui() {

    auto hlayout = new QHBoxLayout();
    this->setLayout(hlayout);

    listview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listview->setSelectionBehavior(QAbstractItemView::SelectRows);
    listview->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

    hlayout->addWidget(listview);
    hlayout->addWidget(this->dtw);
}

void ResWidget::onTableView_currentChanged(QModelIndex &index) {

    this->dtw->showDocDetail(index);
    this->dtw->setVisible(true);
    this->dtw->setMaximumWidth(this->width() * 0.618);
    this->dtw->setMinimumWidth(this->width() * 0.618);
}

void ResWidget::moveToNextResoule() {

    if (listview->model()->rowCount() <= 0) {
        return;
    }
    auto cidx = listview->currentIndex();
    auto r = cidx.row() + 1;
    if (r >= listview->model()->rowCount()) {
        r = 0;
    }

    listview->setCurrentIndex(listview->model()->index(r, 0));
    mdetailRow = r;
//    this->onTableView_currentChanged(QModelIndex());
}


void ResWidget::returnPressed() {
    auto currentIndex = listview->currentIndex();
    if(!currentIndex.isValid()){
        return;
    }

    auto mime =
            currentIndex.data(RecollModel::ModelRoles::Role_MIME_TYPE).toString();
    auto path =
            currentIndex.data(RecollModel::ModelRoles::Role_LOCATION).toString();
    QString aname;
    QStringList args;
    if (mime == "application/x-all") {
        aname="dex";
    } else {
        aname="xdg-open";
    }
    //TODO some item couldn't be open
    qDebug()<<"current item file Location:"<<path;
    QUrl url(path);
    args<<url.toLocalFile();
    QProcess::startDetached(aname,args,"/tmp");
}

void ResWidget::currentMoveUp() {
    auto cidx = listview->model()->index(listview->currentIndex().row() - 1, 0);
    if (cidx.isValid()) {
        listview->setCurrentIndex(cidx);
        mdetailRow = cidx.row();
        this->onTableView_currentChanged(cidx);
    }
}

void ResWidget::currentMoveDown() {
    auto cidx = listview->model()->index(listview->currentIndex().row() + 1, 0);
    if (cidx.row() >=listview->model()->rowCount(QModelIndex())) {
        return;
    }
    mdetailRow = cidx.row();
    qDebug() << "currentMoveDown:Row:" << cidx.row();
    this->onTableView_currentChanged(cidx);
    listview->setCurrentIndex(cidx);

}

void ResWidget::setM_model(UnitedModel *m_model) {
    listview->setModel(m_model);
}

void ResWidget::cleanSearch() {
    listview->clearFocus();
    dtw->hide();

}

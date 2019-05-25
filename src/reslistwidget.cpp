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
        : QWidget(parent), m_model(nullptr), m_ismainres(true) {

    listview = new DListView(this);

    this->dtw = new DetailedWidget();
    QStringList fields;
//    fields << "url"
//           << "title"
//           << "mtype"
//           << "abstract";
    if (!(m_model = new UnitedModel(this)))
        return;
    init_ui();
    init_conn();
}


void ResWidget::init_ui() {

    auto hlayout = new QHBoxLayout();
    this->setLayout(hlayout);

    listview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listview->setSelectionBehavior(QAbstractItemView::SelectRows);
    listview->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
//    listview->setItemDelegate(new SearchItemDelegate(this));
//  proxyModel->setDynamicSortFilter(false);



//  listview->setModel(proxyModel);
    listview->setModel(m_model);

    hlayout->addWidget(listview);
    hlayout->addWidget(this->dtw);
//    hlayout->setStretchFactor(this->listview, 2);
//    hlayout->setStretchFactor(this->dtw, 3);
//    this->dtw->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
//    hlayout->setStretch(0,2);
//    hlayout->setStretch(1,3);

//    this->dtw->setVisible(false);
}

void ResWidget::onTableView_currentChanged(QModelIndex &index) {

//    if (!m_model || !m_model->getDocSource())
//        return;
//    auto index = listview->model()->index(mdetailRow, 0);
//    index=currentFilterModel->mapToSource(index);
//    Rcl::Doc doc;
//    this->m_model->getDocSource()->getDoc(index.row(), doc);
//
//    HighlightData hl;
//    this->m_model->getDocSource()->getTerms(hl);
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
    /*
    auto currentIndex = listview->currentIndex();
    if(!currentIndex.isValid()){
        return;
    }
    currentIndex=currentFilterModel->mapToSource(listview->currentIndex());
    auto vtype = currentIndex.data(RecollModel::Role_VIEW_TYPE).toString();
    if (vtype == "DOT" || vtype == "SECTION") {
        emit filterChanged(
                currentIndex.data(RecollModel::Role_MIME_TYPE).toString());
        this->listview->setModel(filterNone);
        currentFilterModel=filterNone;
        return;
        //TODO
//    proxyModel->setSourceModel(m_model);
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
    args<<path.replace("file://", "");
    QProcess::startDetached(aname,args,"/tmp");
     */
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
    if (cidx.row() >= m_model->rowCount(QModelIndex())) {
        return;
    }
    mdetailRow = cidx.row();
    qDebug() << "currentMoveDown:Row:" << cidx.row();
    this->onTableView_currentChanged(cidx);
    listview->setCurrentIndex(cidx);

}

void ResWidget::setM_model(UnitedModel *m_model) {
    ResWidget::m_model = m_model;
    listview->setModel(m_model);
}

void ResWidget::cleanSearch() {
    listview->clearFocus();
    dtw->hide();

    m_model->cleanSearch();

}

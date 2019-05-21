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



void ResTable::init_conn() {
    connect(this, &ResTable::currentChanged, this, &ResTable::onTableView_currentChanged);
}

ResTable::ResTable(QWidget *parent)
        : QWidget(parent), m_model(nullptr), m_ismainres(true) {

    listview = new DListView(this);
    proxyModel = new MSortFilterProxyModel(this);
    filterNone = new QSortFilterProxyModel(this);

    this->dtw = new DetailedWidget();
    QStringList fields;
//    fields << "url"
//           << "title"
//           << "mtype"
//           << "abstract";
    if (!(m_model = new RecollModel( this)))
        return;
    init_ui();
    init_conn();
}


void ResTable::init_ui() {

    auto hlayout = new QHBoxLayout();
    this->setLayout(hlayout);

    listview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listview->setSelectionBehavior(QAbstractItemView::SelectRows);
    listview->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    listview->setItemDelegate(new SearchItemDelegate(this));
    proxyModel->setSourceModel(this->m_model);
//  proxyModel->setDynamicSortFilter(false);

    filterNone->setFilterRole(RecollModel::ModelRoles::Role_NODISPLAY);
    filterNone->setFilterRegExp("^((?!true).)*$");
    filterNone->setSourceModel(m_model);

//  listview->setModel(proxyModel);
    listview->setModel(filterNone);
    currentFilterModel=filterNone;

    hlayout->addWidget(listview);
    hlayout->addWidget(this->dtw);
//    hlayout->setStretchFactor(this->listview, 2);
//    hlayout->setStretchFactor(this->dtw, 3);
//    this->dtw->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
//    hlayout->setStretch(0,2);
//    hlayout->setStretch(1,3);

    this->dtw->setVisible(false);
}

void ResTable::onTableView_currentChanged() {

    if (!m_model || !m_model->getDocSource())
        return;
    auto index = listview->model()->index(mdetailRow, 0);
    index=currentFilterModel->mapToSource(index);
    Rcl::Doc doc;
    this->m_model->getDocSource()->getDoc(index.row(), doc);

    HighlightData hl;
    this->m_model->getDocSource()->getTerms(hl);
    this->dtw->showDocDetail(index, doc, hl);
    this->dtw->setVisible(true);
    this->dtw->setMaximumWidth(this->width()*0.618);
    this->dtw->setMinimumWidth(this->width()*0.618);
}

void ResTable::moveToNextResoule() {

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
    emit currentChanged();
}

void ResTable::useFilterProxy() {

    listview->setModel(proxyModel);
    currentFilterModel=proxyModel;
}

void ResTable::setDocSource(std::shared_ptr<DocSequence> nsource) {
    if (m_model) {
        m_model->setDocSource(std::move(nsource));
        proxyModel->setMaxItemCount(4);
    }

}

void ResTable::resetSource() {
    setDocSource(std::shared_ptr<DocSequence>(nullptr));
}

void ResTable::readDocSource(bool resetPos) {
    m_model->readDocSource();
    this->dtw->hide();
}

void ResTable::clearSeach() {
    this->resetSource();
    this->readDocSource();
}

void ResTable::returnPressed() {
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
}

void ResTable::currentMoveUp() {
    auto cidx = listview->model()->index(listview->currentIndex().row() - 1, 0);
    if (cidx.isValid()) {
        listview->setCurrentIndex(cidx);
        mdetailRow = cidx.row();
        emit currentChanged();
    }
}

void ResTable::currentMoveDown() {
    auto cidx = listview->model()->index(listview->currentIndex().row() + 1, 0);
    if (cidx.isValid()) {
        mdetailRow = cidx.row();
        emit currentChanged();
        listview->setCurrentIndex(cidx);
    }

}

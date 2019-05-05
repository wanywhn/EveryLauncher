#include "reslistwidget.h"

#include <QDebug>
#include <QHeaderView>
#include <QMessageBox>
#include <QPainter>
#include <QProcess>
#include <QShortcut>
#include <QStyledItemDelegate>
#include <QTextDocument>
#include <QTimer>
#include <QVBoxLayout>
#include <dlistview.h>
#define TEXTINCELLVTRANS -1

///////////////////////////
// ResTable panel methods

// We use a custom delegate to display the cells because the base
// tableview's can't handle rich text to highlight the match terms
class ResTableDelegate : public QStyledItemDelegate {
public:
  ResTableDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

  // We might want to optimize by passing the data to the base
  // method if the text does not contain any term matches. Would
  // need a modif to plaintorich to return the match count (easy),
  // and a way to pass an indicator from data(), a bit more
  // difficult. Anyway, the display seems fast enough as is.
  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const {
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    auto itemType =
        index.data(RecollModel::ModelRoles::Role_VIEW_TYPE).toString();
    auto mimeType=index.data(RecollModel::ModelRoles::Role_MIME_TYPE).toString();
    if (itemType == "SECTION") {
      painter->drawText(opt.rect.adjusted(-1, -1, -1, -1), mimeType);

      if (opt.state & QStyle::State_Selected) {
        painter->fillRect(opt.rect, opt.palette.highlight());
      }
    } else if (itemType == "DOT") {
      painter->drawText(opt.rect.adjusted(-1, -1, -1, -1), "DOT");
      if (opt.state & QStyle::State_Selected) {
        painter->fillRect(opt.rect, opt.palette.highlight());
      }

    } else if (itemType == "ITEM") {

      auto filename =
          index.data(RecollModel::ModelRoles::Role_FILE_NAME).toString();
      if (index.data(RecollModel::ModelRoles::Role_MIME_TYPE).toString() ==
          "application/x-all") {
        // TODO find app icon
        filename =
            index.data(RecollModel::ModelRoles::Role_APP_NAME).toString();
      }
      auto iconpath =
          index.data(RecollModel::ModelRoles::Role_ICON_PATH).toString();
      //    this->liw.setIcon(icon.value<QPixmap>());
      //    this->liw.setTitle(filename.toString());
      //    auto toRender=const_cast<ListItemWidget&>(this->liw).grab();
      QPixmap icon(iconpath);
      if (icon.isNull()) {
        qDebug() << "null icon:" << iconpath;
      }
      icon = icon.scaled(this->sizeHint(option, index));
      QRectF recf(opt.rect);
      //    qDebug()<<filename;
      //    QRectF recf(0,0,50,50);
      if (opt.state & QStyle::State_Selected) {
        painter->fillRect(opt.rect, opt.palette.highlight());
      }
      QRectF iconRectf(opt.rect);
      iconRectf.setSize(icon.size());

      painter->drawPixmap(iconRectf, icon, icon.rect());

      auto textPos = QPointF(iconRectf.topRight());

      textPos.ry() += iconRectf.height() / 2;
      painter->drawText(textPos, filename);
    }
  }

private:
  // QAbstractItemDelegate interface
public:
  QSize sizeHint(const QStyleOptionViewItem &option,
                 const QModelIndex &index) const override {

    //    auto
    //    icon=index.data(RecollModel::ModelRoles::Role_ICON).value<QPixmap>();
    return {50, 50};
  }
};

void ResTable::init() {
}

ResTable::ResTable(QWidget *parent)
    : QWidget(parent), m_model(nullptr), m_detaildocnum(-1), m_ismainres(true) {

  listview = new DListView(this);
  proxyModel = new MSortFilterProxyModel(this);

  this->dtw = new DetailedWidget();
  QStringList fields;
  fields << "url"
         << "title"
         << "mtype"
         << "abstract";
  if (!(m_model = new RecollModel(fields, this)))
    return;
  init_ui();
}

int ResTable::getDetailDocNumOrTopRow() {
  if (m_detaildocnum >= 0)
    return m_detaildocnum;
  return 0;
  //  QModelIndex modelIndex = listViewProgram->indexAt(QPoint(0, 0));
  //  return modelIndex.row();
}

void ResTable::init_ui() {

  auto hlayout = new QHBoxLayout();
  this->setLayout(hlayout);
  //  cw->setLayout(hlayout);

  llayout = new QVBoxLayout();

  llayout->addWidget(listview);
  listview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  listview->setSelectionBehavior(QAbstractItemView::SelectRows);
  listview->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
  listview->setItemDelegate(new ResTableDelegate(this));
  proxyModel->setSourceModel(this->m_model);

  listview->setModel(proxyModel);

  hlayout->addLayout(llayout);
  hlayout->addWidget(this->dtw);
  hlayout->setStretchFactor(llayout, 2);
  hlayout->setStretchFactor(this->dtw, 3);

  this->dtw->setVisible(false);
  //  this->detailedWidget->setWordWrap(true);
}

void ResTable::makeRowVisible(int row) {
  //    LOGDEB("ResTable::showRow(" << row << ")\n");
  QModelIndex modelIndex = m_model->index(row, 0);
  //  listViewProgram->scrollTo(modelIndex, QAbstractItemView::PositionAtTop);
  //  listViewProgram->selectionModel()->clear();
  m_detaildocnum = -1;
}

void ResTable::onTableView_currentChanged() {
  // TODO send user input etc. to detailedWidget for highlitht when display?
  //    LOGDEB2("ResTable::onTableView_currentChanged(" << index.row() << ", "
  //    <<
  //            index.column() << ")\n");

  if (!m_model || !m_model->getDocSource())
    return;
  //  auto proxyModel=vm.at(currentListViewIndex).second;
  //  auto index=proxyModel->index(currentlistViewItemIndex,0);
  //  index=proxyModel->mapToSource(index);
  auto index = listview->currentIndex();
  Rcl::Doc doc;
  this->m_model->getDocSource()->getDoc(index.row(), doc);

  HighlightData hl;
  this->m_model->getDocSource()->getTerms(hl);
  this->dtw->showDocDetail(index, doc, hl);
  //    m_detaildocnum = index.row();
  //    m_detaildoc = doc;
  //    auto t =
  //        this->m_model
  //            ->data(index, RecollModel::ModelRoles::Role_FILE_SIMPLE_CONTENT)
  //            .toString();
  //    this->detailedWidget->setText(t);
  this->dtw->setVisible(true);
  //  }
}

void ResTable::takeFocus() {
  //    LOGDEB("resTable: take focus\n");
  //  listViewProgram->setF/*o*/cus(Qt::ShortcutFocusReason);
}

void ResTable::moveToNextResoule() {
  //    auto cidx=listview->selectionModel();
  //    if(!cidx->hasSelection()){
  //        cidx->select(proxyModel->index(0,0),QItemSelectionModel::Select);
  //    }
  //    while(cidx->selectedRows().at(0).data(RecollModel::Role_VIEW_TYPE).toString()!="ITEM"){
  //        listview->setCurrentIndex();
  //    }
  if (listview->count() <= 0) {
    return;
  }
  auto cidx = listview->currentIndex();
  auto r = cidx.row() + 1;
  if (r >= listview->count()) {
    r = 0;
  }

  listview->setCurrentIndex(listview->model()->index(r, 0));

  onTableView_currentChanged();
}

void ResTable::setDocSource(std::shared_ptr<DocSequence> nsource) {
  if (m_model){
    m_model->setDocSource(nsource);
    proxyModel->setMaxItemCount(4);
  }

  m_detaildocnum = -1;
}

void ResTable::resetSource() {
  setDocSource(std::shared_ptr<DocSequence>(nullptr));
}

void ResTable::readDocSource(bool resetPos) {
  //    LOGDEB("ResTable::readDocSource("  << resetPos << ")\n");
  //    if (resetPos)
  //    listview->verticalScrollBar()->setSliderPosition(0);

  //  for (auto i = 0; i != filterString->size(); ++i) {
  //      vm.at(i).second->setSourceModel(this->m_model);
  //      vm.at(i).first->setModel(vm.at(i).second);
  //  }
  m_model->readDocSource();
  this->dtw->hide();
}

void ResTable::clearSeach() {
  this->resetSource();
  this->readDocSource();
}

void ResTable::returnPressed() {
  auto currentIndex = listview->currentIndex();
  auto vtype=currentIndex.data(RecollModel::Role_VIEW_TYPE).toString();
  if(vtype=="DOT"||vtype=="SECTION"){
      emit filterChanged(currentIndex.data(RecollModel::Role_MIME_TYPE).toString());
      proxyModel->setMaxItemCount(100);
      return ;
  }
  auto mime =
      currentIndex.data(RecollModel::ModelRoles::Role_MIME_TYPE).toString();
  auto path =
      currentIndex.data(RecollModel::ModelRoles::Role_LOCATION).toString();
  static QProcess p;
  //    QProcess *p=new QProcess(this);
  if (mime == "application/x-all") {
    //!!FIXME 打开微信终端下可以，这样貌似不行？
    p.setProgram("dex");
  } else {
    p.setProgram("xdg-open");
  }
  p.setArguments(QStringList(path.replace("file://", "")));
  qDebug() << "P:" << p.program() << " Arg:" << p.arguments();
  p.start();
}

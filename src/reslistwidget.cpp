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
    auto mimeType =
        index.data(RecollModel::ModelRoles::Role_MIME_TYPE).toString();
    if (itemType == "SECTION") {
      painter->drawText(opt.rect.adjusted(-1, -1, -1, -1), mimeType);
      if (opt.state & QStyle::State_Selected) {
        painter->fillRect(opt.rect, opt.palette.highlight());
      }
      return ;
    } else if (itemType == "DOT") {
      painter->drawText(opt.rect.center(), "...");
      if (opt.state & QStyle::State_Selected) {
        painter->fillRect(opt.rect, opt.palette.highlight());
      }
      return ;
    }

//    } else if (itemType == "ITEM") {

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
//    }
  }

private:
  // QAbstractItemDelegate interface
public:
  QSize sizeHint(const QStyleOptionViewItem &option,
                 const QModelIndex &index) const override {

      //TODO hidpi?
    auto itemType =
        index.data(RecollModel::ModelRoles::Role_VIEW_TYPE).toString();
    if(itemType=="SECTION"){


        return {200,25};
    }else if (itemType=="DOT"){

        return {200,25};
    }
    return {50, 50};
  }
};

void ResTable::init_conn() {
    connect(this,&ResTable::currentChanged,this,&ResTable::onTableView_currentChanged);
}

ResTable::ResTable(QWidget *parent)
    : QWidget(parent), m_model(nullptr), mdetailRow(-1), m_ismainres(true) {

  listview = new DListView(this);
  proxyModel = new MSortFilterProxyModel(this);
  filterNone=new QSortFilterProxyModel(this);

  this->dtw = new DetailedWidget();
  QStringList fields;
  fields << "url"
         << "title"
         << "mtype"
         << "abstract";
  if (!(m_model = new RecollModel(fields, this)))
    return;
  init_ui();
  init_conn();
}

int ResTable::getDetailDocNumOrTopRow() {
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
//  listview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  listview->setSelectionBehavior(QAbstractItemView::SelectRows);
  listview->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
  listview->setItemDelegate(new ResTableDelegate(this));
  proxyModel->setSourceModel(this->m_model);
//  proxyModel->setDynamicSortFilter(false);

  filterNone->setFilterRole(RecollModel::ModelRoles::Role_NODISPLAY);
  filterNone->setFilterRegExp("^((?!true).)*$");
  filterNone->setSourceModel(m_model);

//  listview->setModel(proxyModel);
  listview->setModel(filterNone);

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
}

void ResTable::onTableView_currentChanged() {

  if (!m_model || !m_model->getDocSource())
    return;
  auto index = listview->model()->index(mdetailRow,0);
  Rcl::Doc doc;
  this->m_model->getDocSource()->getDoc(index.row(), doc);

  HighlightData hl;
  this->m_model->getDocSource()->getTerms(hl);
  this->dtw->showDocDetail(index, doc, hl);
  this->dtw->setVisible(true);
  //  }
}

void ResTable::takeFocus() {
  //    LOGDEB("resTable: take focus\n");
  //  listViewProgram->setF/*o*/cus(Qt::ShortcutFocusReason);
}

void ResTable::moveToNextResoule() {

  if (listview->model()->rowCount() <= 0) {
    return;
  }
  auto cidx = listview->currentIndex();
  auto r = cidx.row() + 1;
//  qDebug()<<"current idx:"<<cidx.row()+1;
//   qDebug()<<"model data"<<listview->model()->index(r,0).data(RecollModel::ModelRoles::Role_MIME_TYPE);
  if (r >= listview->model()->rowCount()) {
    r = 0;
  }

  listview->setCurrentIndex(listview->model()->index(r, 0));
  mdetailRow=r;
  emit currentChanged();
}

void ResTable::useFilterProxy()
{

    listview->setModel(proxyModel);
}

void ResTable::setDocSource(std::shared_ptr<DocSequence> nsource) {
  if (m_model) {
    m_model->setDocSource(nsource);
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
  auto vtype = currentIndex.data(RecollModel::Role_VIEW_TYPE).toString();
  if (vtype == "DOT" || vtype == "SECTION") {
    emit filterChanged(
        currentIndex.data(RecollModel::Role_MIME_TYPE).toString());
    this->listview->setModel(filterNone);
      return ;
//    proxyModel->setMaxItemCount(100);
//    proxyModel->sort(0);
    //TODO
//    proxyModel->setSourceModel(m_model);
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

void ResTable::currentMoveUp()
{
    auto cidx=listview->model()->index(listview->currentIndex().row()-1,0);
    if(cidx.isValid()){
   listview->setCurrentIndex(cidx);
  mdetailRow=cidx.row();
  emit currentChanged();
    }
}

void ResTable::currentMoveDown()
{
    auto cidx=listview->model()->index(listview->currentIndex().row()+1,0);
    if(cidx.isValid()){
  mdetailRow=cidx.row();
  emit currentChanged();
   listview->setCurrentIndex(cidx);
    }

}

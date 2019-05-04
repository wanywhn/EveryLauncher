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
    auto filename =
        index.data(RecollModel::ModelRoles::Role_FILE_NAME).toString();
    if (index.data(RecollModel::ModelRoles::Role_MIME_TYPE).toString() ==
        "application/x-all") {
      // TODO find app icon
      filename = index.data(RecollModel::ModelRoles::Role_APP_NAME).toString();
    }
     auto iconpath=index.data(RecollModel::ModelRoles::Role_ICON_PATH).toString();
    //    this->liw.setIcon(icon.value<QPixmap>());
    //    this->liw.setTitle(filename.toString());
    //    auto toRender=const_cast<ListItemWidget&>(this->liw).grab();
     QPixmap icon(iconpath);
     icon=icon.scaled(this->sizeHint(option,index));
    QRectF recf(opt.rect);
    //    qDebug()<<filename;
    //    QRectF recf(0,0,50,50);
    if (opt.state & QStyle::State_Selected) {
      painter->fillRect(opt.rect, opt.palette.highlight());
    }
    QRectF iconRectf(opt.rect);
    iconRectf.setSize(icon.size());

    painter->drawPixmap(iconRectf,icon,icon.rect());

    auto textPos = QPointF(iconRectf.topRight());

    textPos.ry() += iconRectf.height() / 2;
    painter->drawText(textPos, filename);
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
  //  listViewProgram->setModel(m_model);
  //  listViewProgram->setMouseTracking(true);
  //  listViewProgram->setContextMenuPolicy(Qt::CustomContextMenu);

  //    QHeaderView *header = listview->horizontalHeader();
  //    if (header) {
  //    if (int(prefs.restableColWidths.size()) == header->count()) {
  //        for (int i = 0; i < header->count(); i++) {
  //        header->resizeSection(i, 12);
  //        }
  //    }
  //    header->setSortIndicatorShown(true);
  //    header->setSortIndicator(-1, Qt::AscendingOrder);
  //    header->setContextMenuPolicy(Qt::CustomContextMenu);
  //    header->setStretchLastSection(1);
  //    connect(header, SIGNAL(sectionResized(int,int,int)),
  //        this, SLOT(saveColState()));
  //    connect(header, SIGNAL(customContextMenuRequested(const QPoint&)),
  //        this, SLOT(createHeaderPopupMenu(const QPoint&)));
  //    }
  //    header->setSectionsMovable(true);

  //    header = listview->verticalHeader();
  //    if (header) {
  //    header->setDefaultSectionSize(QApplication::fontMetrics().height() +
  //                      ROWHEIGHTPAD);
  //    }

  //  QKeySequence seq("Esc");
  //  QShortcut *sc = new QShortcut(seq, this);
  //    connect(sc, &QShortcut::activated,
  //        listview->selectionModel(), &QItemSelectionModel::clear);
  // TODO here emit signal to dispay right widget

  //  connect(listViewProgram->selectionModel(),
  //  &QItemSelectionModel::currentChanged,
  //          this, &ResTable::onTableView_currentChanged);

  //    connect(listview, &QAbstractItemView::doubleClicked,
  //            this, &ResTable::onDoubleClick);

  // QSettings settings;
  // QVariant saved = settings.value("resTableSplitterSizes");
  // if (saved != QVariant()) {
  // splitter->restoreState(saved.toByteArray());
  // } else {
  // QList<int> sizes;
  // sizes << 355 << 125;
  // splitter->setSizes(sizes);
  // }

  //    delete textBrowser;
  // m_detail = new ResTableDetailArea(this);
  // m_detail->setReadOnly(true);
  // m_detail->setUndoRedoEnabled(false);
  // m_detail->setOpenLinks(false);
  // signals and slots connections
  // connect(m_detail, SIGNAL(anchorClicked(const QUrl &)),
  //     this, SLOT(linkWasClicked(const QUrl &)));
  // splitter->addWidget(m_detail);
  // splitter->setOrientation(Qt::Vertical);
}

/*
void ResTable::setRclMain(RclMain *m, bool ismain)
{
    m_ismainres = ismain;

    // We allow single selection only in the main table because this
    // may have a mix of file-level docs and subdocs and multisave
    // only works for subdocs
    if (m_ismainres)
    listview->setSelectionMode(QAbstractItemView::SingleSelection);
    else
    listview->setSelectionMode(QAbstractItemView::ExtendedSelection);

}
*/

ResTable::ResTable(QWidget *parent)
    : QWidget(parent), m_model(nullptr), m_detaildocnum(-1), m_ismainres(true) {
  //  this->listViewProgram = new QListView();
  //    this->listViewDoc=new QListView();
  // Store the reg of type
  //  filterString = new QStringList({"application/x-all", "text/*"});
  filterString = new QStringList({"*"});
  for (auto i = 0; i != filterString->size(); ++i) {
    vm.push_back(QPair<DListView *, MSortFilterProxyModel *>(
        new DListView(this), new MSortFilterProxyModel(this)));
  }

  this->dtw = new DetailedWidget();
  currentListViewIndex = 0;
  currentlistViewItemIndex = -1;
  QStringList fields;
  fields << "url"
         << "title"
         << "mtype"
         << "abstract";
  if (!(m_model = new RecollModel(fields, this)))
    return;
  init_ui();
  init();
}

int ResTable::getDetailDocNumOrTopRow() {
  if (m_detaildocnum >= 0)
    return m_detaildocnum;
  return 0;
  //  QModelIndex modelIndex = listViewProgram->indexAt(QPoint(0, 0));
  //  return modelIndex.row();
}

void ResTable::init_ui() {

  //    auto cw=new QWidget(this);
  //    this->setCentralWidget(cw);
  auto hlayout = new QHBoxLayout();
  this->setLayout(hlayout);
  //  cw->setLayout(hlayout);

  llayout = new QVBoxLayout();
  // TODO here add multi view ,using different filter proxy model
  for (auto i = 0; i != filterString->size(); ++i) {

    llayout->addWidget(vm.at(i).first);
    vm.at(i).first->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vm.at(i).first->setAlternatingRowColors(true);
    vm.at(i).first->setSelectionBehavior(QAbstractItemView::SelectRows);
    vm.at(i).first->setSelectionMode(
        QAbstractItemView::SelectionMode::SingleSelection);
    vm.at(i).first->setItemDelegate(new ResTableDelegate(this));

    vm.at(i).first->setModel(this->m_model);

    //    vm.at(i).second->setSourceModel(this->m_model);
    //    vm.at(i).second->setFilterRole(RecollModel::ModelRoles::Role_MIME_TYPE);
    //    vm.at(i).second->setFilterRegExp(filterString->at(i));
    //        vm.at(i).second->setFilterWildcard(filterString->at(i));
    //    vm.at(i).second->setSortRole(RecollModel::ModelRoles::Role_RELEVANCY);
    //    vm.at(i).second->setDynamicSortFilter(false);
    //        vm.at(i).first->setModel(vm.at(i).second);

    connect(vm.at(i).second, &MSortFilterProxyModel::itemCountChanged,
            [this, i](int count) {
              qDebug() << "se cout:" << count;
              auto view = this->vm.at(i).first;
              // TODO height?
              view->resize(view->width(), count * 70);
            });
  }

  hlayout->addLayout(llayout);
  hlayout->addWidget(this->dtw);
  hlayout->setStretchFactor(llayout, 1);
  hlayout->setStretchFactor(this->dtw, 2);

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
  auto index = vm.at(currentListViewIndex)
                   .first->model()
                   ->index(currentlistViewItemIndex, 0);
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
  auto listIndex = currentListViewIndex;
  auto itemIndex = currentlistViewItemIndex + 1;
  int targetListViewIdex = -1;
  for (int idx = listIndex; idx < llayout->count(); ++idx) {
    auto view = qobject_cast<QListView *>(llayout->itemAt(idx)->widget());
    if (itemIndex < view->model()->rowCount()) {
      // find
      targetListViewIdex = idx;
      break;
    } else {
      // current listview end,goto next and start from 0
      itemIndex = 0;
    }
  }

  if (targetListViewIdex == -1) {
    // iterate to end still not found,
    bool ffnz = false;
    for (auto i = 0; i != llayout->count(); ++i) {
      auto view = qobject_cast<QListView *>(llayout->itemAt(i)->widget());
      if (ffnz) {
        view->setCurrentIndex(view->model()->index(-1, 0));
        continue;
      }
      if (view->model()->rowCount() > 0) {
        auto ci = view->model()->index(i, 0);
        view->setCurrentIndex(ci);
        targetListViewIdex = i;
        itemIndex = 0;
        ffnz = true;
        continue;
      }
    }
    //    if (ffnz) {
    //    } else {
    //      currentListViewIndex = 0;
    //      currentlistViewItemIndex = -1;
    //    }
  }
  //  } else {
  // find the "next"
  currentListViewIndex = targetListViewIdex == -1 ? 0 : targetListViewIdex;
  currentlistViewItemIndex = itemIndex;
  for (auto i = 0; i != llayout->count(); ++i) {
    auto view = qobject_cast<QListView *>(llayout->itemAt(i)->widget());
    if (i != currentListViewIndex) {
      view->setCurrentIndex(view->model()->index(-1, 0));
      continue;
    }
    auto ci = view->model()->index(currentlistViewItemIndex, 0);
    view->setCurrentIndex(ci);
    //    }
  }
  onTableView_currentChanged();
}

void ResTable::setDocSource(std::shared_ptr<DocSequence> nsource) {
  //    LOGDEB("ResTable::setDocSource\n");
  if (m_model)
    m_model->setDocSource(nsource);
  m_detaildocnum = -1;
}

void ResTable::resetSource() {
  //    LOGDEB("ResTable::resetSource\n");
  setDocSource(std::shared_ptr<DocSequence>(nullptr));
}

/*
// This is called when the sort order is changed from another widget
void ResTable::onSortDataChanged(DocSeqSortSpec spec)
{
//    LOGDEB("ResTable::onSortDataChanged: [" << spec.field << "] desc " <<
//           spec.desc << "\n");
//    QHeaderView *header = listview->horizontalHeader();
//    if (!header || !m_model)
//    return;

    // Check if the specified field actually matches one of columns
    // and set indicator
    m_model->setIgnoreSort(true);
    bool matched = false;
    const vector<string> fields = m_model->getFields();
    for (unsigned int i = 0; i < fields.size(); i++) {
    if (!spec.field.compare(m_model->baseField(fields[i]))) {
//        header->setSortIndicator(i, spec.desc ?
//                     Qt::DescendingOrder : Qt::AscendingOrder);
        matched = true;
    }
    }
    if (!matched)
//    header->setSortIndicator(-1, Qt::AscendingOrder);
    m_model->setIgnoreSort(false);
}


*/
void ResTable::readDocSource(bool resetPos) {
  //    LOGDEB("ResTable::readDocSource("  << resetPos << ")\n");
  //    if (resetPos)
  //    listview->verticalScrollBar()->setSliderPosition(0);

  //  for (auto i = 0; i != filterString->size(); ++i) {
  //      vm.at(i).second->setSourceModel(this->m_model);
  //      vm.at(i).first->setModel(vm.at(i).second);
  //  }
  m_model->readDocSource();
  //  m_detaildocnum = -1;
  this->currentListViewIndex = 0;
  this->currentlistViewItemIndex = -1;
  this->dtw->hide();
}

void ResTable::clearSeach() {
  this->resetSource();
    this->readDocSource();
}

void ResTable::returnPressed()
{
    auto currentIndex=vm.at(currentListViewIndex).first->currentIndex();
    auto mime=currentIndex.data(RecollModel::ModelRoles::Role_MIME_TYPE).toString();
    auto path=currentIndex.data(RecollModel::ModelRoles::Role_LOCATION).toString();
    static QProcess p;
//    QProcess *p=new QProcess(this);
    if(mime=="application/x-all"){
        //!!FIXME 打开微信终端下可以，这样貌似不行？
        p.setProgram("dex");
    }else{
        p.setProgram("xdg-open");
    }
    p.setArguments(QStringList(path.replace("file://","")));
    qDebug()<<"P:"<<p.program()<<" Arg:"<<p.arguments();
    p.start();

}

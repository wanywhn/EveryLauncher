#include "reslistwidget.h"

#include <QDebug>
#include <QHeaderView>
#include <QMessageBox>
#include <QPainter>
#include <QShortcut>
#include <QStyledItemDelegate>
#include <QTextDocument>
#include <QTimer>
#include <QVBoxLayout>
#define TEXTINCELLVTRANS -1

class PlainToRichQtReslist : public PlainToRich {
public:
  virtual ~PlainToRichQtReslist() {}
  virtual string startMatch(unsigned int) {
    return string("<span style='") + "TODO" + string("'>");
  }
  virtual string endMatch() { return string("</span>"); }
};

//////////////////////////////////////////////////////////////////////////////
//// Data model methods
////

// Routines used to extract named data from an Rcl::Doc. The basic one
// just uses the meta map. Others (ie: the date ones) need to do a
// little processing
static QString gengetter(const string &fld, const Rcl::Doc &doc) {
  const auto it = doc.meta.find(fld);
  if (it == doc.meta.end()) {
    return "";
  }
  return QString::fromStdString(it->second);
}

static QPixmap picgetter(const Rcl::Doc &doc) {
  const auto it = doc.meta.find("mtype");
  if (it == doc.meta.end()) {
    return QPixmap();
  }
  return QPixmap();
}
static QString sizegetter(const string &fld, const Rcl::Doc &doc) {
  const auto it = doc.meta.find(fld);
  if (it == doc.meta.end()) {
    return "";
  }
  int64_t size = atoll(it->second.c_str());
  return QString::fromStdString(displayableBytes(size) + " (" + it->second +
                                ")");
}

static QString dategetter(const string &, const Rcl::Doc &doc) {
  string sdate;
  if (!doc.dmtime.empty() || !doc.fmtime.empty()) {
    char datebuf[100];
    datebuf[0] = 0;
    time_t mtime = doc.dmtime.empty() ? atoll(doc.fmtime.c_str())
                                      : atoll(doc.dmtime.c_str());
    struct tm *tm = localtime(&mtime);
    strftime(datebuf, 99, "%Y-%m-%d", tm);
    //        transcode(datebuf, sdate, "", "UTF-8");
  }
  return QString::fromStdString(sdate);
}

static QString datetimegetter(const string &, const Rcl::Doc &doc) {
  char datebuf[100];
  datebuf[0] = 0;
  if (!doc.dmtime.empty() || !doc.fmtime.empty()) {
    time_t mtime = doc.dmtime.empty() ? atoll(doc.fmtime.c_str())
                                      : atoll(doc.dmtime.c_str());
    struct tm *tm = localtime(&mtime);
    //    strftime(datebuf, 99, prefs.creslistdateformat.c_str(), tm);
  }
  return datebuf;
}

// Static map to translate from internal column names to displayable ones
map<string, QString> RecollModel::o_displayableFields;

FieldGetter *RecollModel::chooseGetter(const string &field) {
  if (!stringlowercmp("date", field))
    return dategetter;
  else if (!stringlowercmp("datetime", field))
    return datetimegetter;
  else if (!stringlowercmp("bytes", field.substr(1)))
    return sizegetter;
  else
    return gengetter;
}

string RecollModel::baseField(const string &field) {
  if (!stringlowercmp("date", field) || !stringlowercmp("datetime", field))
    return "mtime";
  else
    return field;
}

RecollModel::RecollModel(const QStringList fields, QObject *parent)
    : QAbstractListModel(parent), m_ignoreSort(false) {
  // Initialize the translated map for column headers
  o_displayableFields["abstract"] = tr("Abstract");
  o_displayableFields["author"] = tr("Author");
  o_displayableFields["dbytes"] = tr("Document size");
  o_displayableFields["dmtime"] = tr("Document date");
  o_displayableFields["fbytes"] = tr("File size");
  o_displayableFields["filename"] = tr("File name");
  o_displayableFields["fmtime"] = tr("File date");
  o_displayableFields["ipath"] = tr("Ipath");
  o_displayableFields["keywords"] = tr("Keywords");
  o_displayableFields["mtype"] = tr("MIME type");
  o_displayableFields["origcharset"] = tr("Original character set");
  o_displayableFields["relevancyrating"] = tr("Relevancy rating");
  o_displayableFields["title"] = tr("Title");
  o_displayableFields["url"] = tr("URL");
  o_displayableFields["mtime"] = tr("Mtime");
  o_displayableFields["date"] = tr("Date");
  o_displayableFields["datetime"] = tr("Date and time");

  // Add dynamic "stored" fields to the full column list. This
  // could be protected to be done only once, but it's no real
  // problem
  if (theconfig) {
    const set<string> &stored = theconfig->getStoredFields();
    for (set<string>::const_iterator it = stored.begin(); it != stored.end();
         it++) {
      if (o_displayableFields.find(*it) == o_displayableFields.end()) {
        o_displayableFields[*it] = QString::fromUtf8(it->c_str());
      }
    }
  }

  // Construct the actual list of column names
  for (QStringList::const_iterator it = fields.begin(); it != fields.end();
       it++) {
    m_fields.push_back((const char *)(it->toUtf8()));
    m_getters.push_back(chooseGetter(m_fields.back()));
  }

  //    g_hiliter.set_inputhtml(false);
}

RecollModel::~RecollModel() {}

int RecollModel::rowCount(const QModelIndex &) const {
  if (!m_source)
    return 0;
  auto cnt = m_source->getResCnt();
  return cnt;
}

void RecollModel::readDocSource() {
  beginResetModel();
  endResetModel();
}

void RecollModel::setDocSource(std::shared_ptr<DocSequence> nsource) {
  if (!nsource) {
    m_source = std::shared_ptr<DocSequence>();
  } else {

    //        auto theconfig=new RclConfig();
    m_source = std::shared_ptr<DocSequence>(new DocSource(theconfig, nsource));
    m_hdata.clear();
    m_source->getTerms(m_hdata);
  }
}

QVariant RecollModel::headerData(int idx, Qt::Orientation orientation,
                                 int role) const {
  //    LOGDEB2("RecollModel::headerData: idx " << idx << " orientation " <<
  //            (orientation == Qt::Vertical ? "vertical":"horizontal") <<
  //            " role " << role << "\n");
  if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
    return idx;
  }
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole &&
      idx < int(m_fields.size())) {
    map<string, QString>::const_iterator it =
        o_displayableFields.find(m_fields[idx]);
    if (it == o_displayableFields.end())
      return QString::fromUtf8(m_fields[idx].c_str());
    else
      return it->second;
  }
  return QVariant();
}

QVariant RecollModel::data(const QModelIndex &index, int role) const {
  //    LOGDEB2("RecollModel::data: row " << index.row() << " col " <<
  //            index.column() << " role " << role << "\n");
  if (!m_source || !index.isValid() || role < Qt::UserRole) {
    //  if (!m_source || !index.isValid() || role != Qt::DisplayRole) {
    //      index.column() >= int(m_fields.size())) {
    return QVariant();
  }
  //  return QVariant("test");

  Rcl::Doc doc;
  if (!m_source->getDoc(index.row(), doc)) {
    return QVariant();
  }
  QVariant var;
  switch (role) {
  case Role_FILE_NAME:
    var = gengetter("filename", doc);
    break;

  case Role_LOCATION:
    var = gengetter("url", doc);

    break;
  case Role_FILE_SIMPLE_CONTENT:
    var = gengetter("abstract", doc);

    break;
  case Role_MIME_TYPE:
    var = gengetter("mtype", doc);
    break;
  case Role_RELEVANCY:
    var = gengetter("relevancyrating", doc);
    break;
  case Role_ICON:

    var = picgetter(doc);

    break;
  default:
    return "error Role";
  }
  return var;

  //  string colname = m_fields[index.column()];

  //  string data = m_getters[index.column()](colname, doc);

  // TODO highlight
  //  list<string> lr;
  //    g_hiliter.plaintorich(data, lr, m_hdata);
  //  return QString::fromUtf8(lr.front().c_str());
}

// This gets called when the column headers are clicked
void RecollModel::sort(int column, Qt::SortOrder order) {
  if (m_ignoreSort) {
    return;
  }
  //    LOGDEB("RecollModel::sort(" << column << ", " << order << ")\n");

  DocSeqSortSpec spec;
  if (column >= 0 && column < int(m_fields.size())) {
    spec.field = m_fields[column];
    if (!stringlowercmp("relevancyrating", spec.field) &&
        order != Qt::AscendingOrder) {
      QMessageBox::warning(0, "Recoll", tr("Can't sort by inverse relevance"),
                           QMessageBox::Ok);
      // TODO
      //            QTimer::singleShot(0, m_table, SLOT(resetSort()));
      return;
    }
    if (!stringlowercmp("date", spec.field) ||
        !stringlowercmp("datetime", spec.field))
      spec.field = "mtime";
    spec.desc = order == Qt::AscendingOrder ? false : true;
  }
  emit sortDataChanged(spec);
}

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
    auto icon = index.data(RecollModel::ModelRoles::Role_ICON).value<QPixmap>();
    //    this->liw.setIcon(icon.value<QPixmap>());
    //    this->liw.setTitle(filename.toString());
    //    auto toRender=const_cast<ListItemWidget&>(this->liw).grab();
    QRectF recf(opt.rect);
    //    qDebug()<<filename;
    //    QRectF recf(0,0,50,50);
    if (opt.state & QStyle::State_Selected) {
      painter->fillRect(opt.rect, opt.palette.highlight());
    }
    painter->drawPixmap(recf, icon, icon.rect());
    //    painter->setPen(opt.palette.text().color());
    auto textpos = QPointF(icon.size().width(), recf.y() + recf.height() / 2);
    painter->drawText(textpos, filename);
    //    QStyledItemDelegate::paint(painter,option,index);

    /*
    if (value.isValid() && !value.isNull()) {
      QString text = value.toString();
      if (!text.isEmpty()) {
        QTextDocument document;
        painter->save();
        if (opt.state & QStyle::State_Selected) {
          painter->fillRect(opt.rect, opt.palette.highlight());
          // Set the foreground color. The pen approach does
          // not seem to work, probably it's reset by the
          // textdocument. Couldn't use
          // setdefaultstylesheet() either. the div thing is
          // an ugly hack. Works for now
#if 0
            QPen pen = painter->pen();
            pen.setBrush(opt.palette.brush(QPalette::HighlightedText));
            painter->setPen(pen);
#else
          text = QString::fromUtf8("<div style='color: white'> ") + text +
                 QString::fromUtf8("</div>");
#endif
        }
        painter->setClipRect(option.rect);
        QPoint where = option.rect.topLeft();
        where.ry() += TEXTINCELLVTRANS;
        painter->translate(where);
        document.setHtml(text);
        document.drawContents(painter);
        painter->restore();
        return;
      }
    }
    QStyledItemDelegate::paint(painter, option, index);
    */
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
static PlainToRichQtReslist g_hiliter;

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
    vm.push_back(QPair<QListView *, MSortFilterProxyModel *>(
        new QListView, new MSortFilterProxyModel(this)));
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

  auto hlayout = new QHBoxLayout();
  this->setLayout(hlayout);

  llayout = new QVBoxLayout();
  // TODO here add multi view ,using different filter proxy model
  for (auto i = 0; i != filterString->size(); ++i) {

    llayout->addWidget(vm.at(i).first);
    vm.at(i).first->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vm.at(i).first->setSelectionBehavior(QAbstractItemView::SelectRows);
    vm.at(i).first->setSelectionMode(
        QAbstractItemView::SelectionMode::SingleSelection);
    vm.at(i).first->setItemDelegate(new ResTableDelegate(this));

//    vm.at(i).first->setModel(this->m_model);

    //    vm.at(i).second->setSourceModel(this->m_model);
    //    vm.at(i).second->setFilterRole(RecollModel::ModelRoles::Role_MIME_TYPE);
    //    vm.at(i).second->setFilterRegExp(filterString->at(i));
        vm.at(i).second->setFilterWildcard(filterString->at(i));
    //    vm.at(i).second->setSortRole(RecollModel::ModelRoles::Role_RELEVANCY);
    //    vm.at(i).second->setDynamicSortFilter(false);
        vm.at(i).first->setModel(vm.at(i).second);

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
  //    LOGDEB2("ResTable::onTableView_currentChanged(" << index.row() << ", "
  //    <<
  //            index.column() << ")\n");

  if (!m_model || !m_model->getDocSource())
    return;
  auto proxyModel=vm.at(currentListViewIndex).second;
  auto index=proxyModel->index(currentlistViewItemIndex,0);
  index=proxyModel->mapToSource(index);
//  Rcl::Doc doc;
//  m_model->getDocSource()->getDoc(index.row(), doc);
  this->dtw->showDocDetail(index);
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
  qDebug() << "move to next";
  //  auto childerns = llayout->children();
  //  auto childerns = llayout->
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
  currentListViewIndex = targetListViewIdex;
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
  m_detaildocnum = -1;
}

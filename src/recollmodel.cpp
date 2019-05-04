#include "recollmodel.h"

#include <bits/stl_list.h>
#include <bits/stl_map.h>

#include <QDebug>
#include <QIcon>
#include <QMessageBox>
#include <QPixmap>
#include <plaintorich.h>

class PlainToRichQtReslist : public PlainToRich {
public:
  virtual ~PlainToRichQtReslist() {}
  virtual string startMatch(unsigned int) {
    return string("<span style='") + "TODO" + string("'>");
  }
  virtual string endMatch() { return string("</span>"); }
};
static PlainToRichQtReslist g_hiliter;

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

// static QPixmap picgetter(const Rcl::Doc &doc) {
//  const auto it = doc.meta.find("mtype");
//  if (it == doc.meta.end()) {
//    return QPixmap();
//  }
//  return QPixmap();
//}
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

  g_hiliter.set_inputhtml(false);
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
  std::list<std::string> lr;
  //  return QString::fromUtf8(lr.front().c_str());
  if (!m_source->getDoc(index.row(), doc)) {
    return QVariant();
  }
  QVariant var;
  switch (role) {
  case Role_FILE_NAME: {
    var = gengetter("filename", doc);
    break;
  }
  case Role_LOCATION: {
    var = gengetter("url", doc);
    break;
  }
  case Role_FILE_SIMPLE_CONTENT: {
    var = gengetter("abstract", doc);
    g_hiliter.plaintorich(var.toString().toStdString(), lr, m_hdata);
    var = QString::fromUtf8(lr.front().c_str());
    break;
  }
  case Role_MIME_TYPE: {
    var = gengetter("mtype", doc);
    break;
  }
  case Role_RELEVANCY: {
    var = gengetter("relevancyrating", doc);
    break;
  }
    // the default cat icon
  case Role_ICON_PATH: {
    auto mtype = gengetter("mtype", doc);
    if (mtype != "application/x-all") {
      std::string apptag;
      doc.getmeta(Rcl::Doc::keyapptg, &apptag);
      var = QString::fromStdString(
          theconfig->getMimeIconPath(doc.mimetype, apptag));
    } else {
      var = gengetter("appicon", doc);
    }
    break;
  }
  case Role_APP_NAME: {
    var = gengetter("appname", doc);
    break;
  }
  case Role_APP_COMMENT: {
    var = gengetter("appcomment", doc);
    break;
  }
  }
  return var;
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

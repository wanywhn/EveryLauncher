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

#include "guiutils.h"
#include "internfile.h"
#include "log.h"
#include "preview_load.h"
#include "preview_plaintorich.h"
#include "preview_w.h"
#include "smallut.h"
#include <QApplication>

extern

    RclConfig *theconfig;

    Preview::Preview()// Search terms etc. for highlighting
        : DetailedW(nullptr) {
        setSupportedMimeType=std::set<std::string>({
//        setSupportedMimeType.fromList({

                                     "text/plain",
                                                       "application/msword",

                                      });
        init();
    }

    void Preview::init() {
        setObjectName("Preview");
        QVBoxLayout *previewLayout = new QVBoxLayout(this);

  pvEdit = new PreviewTextEdit(this, "pvEdit", this);
  pvEdit->setReadOnly(true);
  pvEdit->setUndoRedoEnabled(false);
  pvEdit->setText("23123");
  previewLayout->addWidget(pvEdit);
  this->setLayout(previewLayout);
}

void Preview::showDoc(Rcl::Doc doc) {
  LOGDEB("Preview::makeDocCurrent: " << doc.url << "\n");

  if (m_loading) {
    LOGERR("Already loading\n");
    return;
  }
  loadDocInCurrentTab(doc, 1);
  return;
}

/*
  Code for loading a file into an pvEdit window. The operations that
  we call have no provision to indicate progression, and it would be
  complicated or impossible to modify them to do so (Ie: for external
  format converters).

  We implement a complicated and ugly mechanism based on threads to indicate
  to the user that the app is doing things: lengthy operations are done in
  threads and we update a progress indicator while they proceed (but we have
  no estimate of their total duration).

  It might be possible, but complicated (need modifications in
  handler) to implement a kind of bucket brigade, to have the
  beginning of the text displayed faster
*/

// Insert into pvEdit by chunks so that the top becomes visible
// earlier for big texts. This provokes some artifacts (adds empty line),
// so we can't set it too low.
#define CHUNKL 500 * 1000

// Make sure we don't ever reenter loadDocInCurrentTab: note that I
// don't think it's actually possible, this must be the result of a
// misguided debug session.
class LoadGuard {
  bool *m_bp;

public:
  LoadGuard(bool *bp) {
    m_bp = bp;
    *m_bp = true;
  }
  ~LoadGuard() { *m_bp = false; }
};

bool Preview::loadDocInCurrentTab(const Rcl::Doc &idoc, int docnum) {
  LOGDEB1("Preview::loadDocInCurrentTab()\n");

  LoadGuard guard(&m_loading);

  QString msg = QString("Loading: %1 (size %2 bytes)")
                    .arg(QString::fromLocal8Bit(idoc.url.c_str()))
                    .arg(QString::fromUtf8(idoc.fbytes.c_str()));

  QProgressDialog progress(msg, tr("Cancel"), 0, 0, this);
  progress.setMinimumDuration(2000);
  QEventLoop loop;
  QTimer tT;
  tT.setSingleShot(true);
  connect(&tT, SIGNAL(timeout()), &loop, SLOT(quit()));

  ////////////////////////////////////////////////////////////////////////
  // Load and convert document
  // idoc came out of the index data (main text and some fields missing).
  // fdoc is the complete one what we are going to extract from storage.
  LoadThread lthr(theconfig, idoc, true, this);
  connect(&lthr, SIGNAL(finished()), &loop, SLOT(quit()));

  lthr.start();
  for (int i = 0;; i++) {
    tT.start(1000);
    loop.exec();
    if (lthr.isFinished())
      break;
    if (progress.wasCanceled()) {
    }
    if (i == 1)
      progress.show();
  }

  if (lthr.status != 0) {
    progress.close();
    QString explain;
    if (!lthr.missing.empty()) {
      explain = QString::fromUtf8("<br>") + tr("Missing helper program: ") +
                QString::fromLocal8Bit(lthr.missing.c_str());
      QMessageBox::warning(0, "Recoll",
                           tr("Can't turn doc into internal "
                              "representation for ") +
                               lthr.fdoc.mimetype.c_str() + explain);
    } else {
      if (progress.wasCanceled()) {
        // QMessageBox::warning(0, "Recoll", tr("Canceled"));
      } else {
        QMessageBox::warning(0, "Recoll", tr("Error while loading file"));
      }
    }

    return false;
  }
  // Reset config just in case.
  theconfig->setKeyDir("");

  ////////////////////////////////////////////////////////////////////////
  // Create preview text: highlight search terms
  // We don't do the highlighting for very big texts: too long. We
  // should at least do special char escaping, in case a '&' or '<'
  // somehow slipped through previous processing.
  bool highlightTerms =
      lthr.fdoc.text.length() < (unsigned long)2 * 1024 * 1024;

  // Final text is produced in chunks so that we can display the top
  // while still inserting at bottom

  pvEdit->m_plaintorich->clear();

  // For an actual html file, if we want to have the images and
  // style loaded in the preview, we need to set the search
  // path. Not too sure this is a good idea as I find them rather
  // distracting when looking for text, esp. with qtextedit
  // relatively limited html support (text sometimes get hidden by
  // images).
#if 0
    string path = fileurltolocalpath(idoc.url);
    if (!path.empty()) {
        path = path_getfather(path);
        QStringList paths(QString::fromLocal8Bit(path.c_str()));
        pvEdit->setSearchPaths(paths);
    }
#endif

  pvEdit->setHtml("");
  pvEdit->m_format = Qt::RichText;
  bool inputishtml = !lthr.fdoc.mimetype.compare("text/html");
  QStringList qrichlst;
  pvEdit->m_plaintorich->set_activatelinks(true);

#if 1
  if (highlightTerms) {
    progress.setLabelText(tr("Creating preview text"));
    qApp->processEvents();

    if (inputishtml) {
      LOGDEB1("Preview: got html " << lthr.fdoc.text << "\n");
      pvEdit->m_plaintorich->set_inputhtml(true);
    } else {
      LOGDEB1("Preview: got plain " << lthr.fdoc.text << "\n");
      pvEdit->m_plaintorich->set_inputhtml(false);
    }

    ToRichThread rthr(lthr.fdoc.text, m_hData, pvEdit->m_plaintorich, qrichlst,
                      this);
    connect(&rthr, SIGNAL(finished()), &loop, SLOT(quit()));
    rthr.start();

    for (;;) {
      tT.start(1000);
      loop.exec();
      if (rthr.isFinished())
        break;
      if (progress.wasCanceled()) {
      }
    }

    // Conversion to rich text done
  } else {
    LOGDEB("Preview: no highlighting, loading " << lthr.fdoc.text.size()
                                                << " bytes\n");
    // No plaintorich() call.  In this case, either the text is
    // html and the html quoting is hopefully correct, or it's
    // plain-text and there is no need to escape special
    // characters. We'd still want to split in chunks (so that the
    // top is displayed faster), but we must not cut tags, and
    // it's too difficult on html. For text we do the splitting on
    // a QString to avoid utf8 issues.
    QString qr =
        QString::fromUtf8(lthr.fdoc.text.c_str(), lthr.fdoc.text.length());
    int l = 0;
    if (inputishtml) {
      qrichlst.push_back(qr);
    } else {
      pvEdit->setPlainText("");
      pvEdit->m_format = Qt::PlainText;
      for (int pos = 0; pos < (int)qr.length(); pos += l) {
        l = MIN(CHUNKL, qr.length() - pos);
        qrichlst.push_back(qr.mid(pos, l));
      }
    }
  }
#else // For testing qtextedit bugs...
  highlightTerms = true;
  const char *textlist[] = {
      "Du plain text avec un\n <termtag>termtag</termtag> fin de ligne:",
      "texte apres le tag\n",
  };
  const int listl = sizeof(textlist) / sizeof(char *);
  for (int i = 0; i < listl; i++)
    qrichlst.push_back(QString::fromUtf8(textlist[i]));
#endif

  ///////////////////////////////////////////////////////////
  // Load text into pvEdit window.
  progress.setLabelText(tr("Loading preview text into pvEdit"));
  qApp->processEvents();
  for (QStringList::iterator it = qrichlst.begin(); it != qrichlst.end();
       it++) {
    qApp->processEvents();

    auto s=*it;
    pvEdit->append(*it);
    // We need to save the rich text for printing, the pvEdit does
    // not do it consistently for us.
    pvEdit->m_richtxt.append(*it);

    if (progress.wasCanceled()) {
      pvEdit->append("<b>Cancelled !</b>");
      LOGDEB("loadDocInCurrentTab: cancelled in pvEdit load\n");
      break;
    }
  }

  progress.close();
  pvEdit->m_curdsp = PreviewTextEdit::PTE_DSPTXT;

  ////////////////////////////////////////////////////////////////////////
  // Finishing steps

  // Maybe the text was actually empty ? Switch to fields then. Else free-up
  // the text memory in the loaded document. We still have a copy of the text
  // in pvEdit->m_richtxt
  bool textempty = lthr.fdoc.text.empty();
  if (!textempty)
    lthr.fdoc.text.clear();
  pvEdit->m_fdoc = lthr.fdoc;
  pvEdit->m_dbdoc = idoc;
  if (textempty)
    pvEdit->displayFields();

  // If this is an image, display it instead of the text.
  if (!idoc.mimetype.compare(0, 6, "image/")) {
    string fn = fileurltolocalpath(idoc.url);
    theconfig->setKeyDir(fn.empty() ? "" : path_getfather(fn));

    // We want a real file, so if this comes from data or we have
    // an ipath, create it.
    if (fn.empty() || !idoc.ipath.empty()) {
      TempFile temp = lthr.tmpimg;
      if (temp) {
        LOGDEB1("Preview: load: got temp file from internfile\n");
      } else if (!FileInterner::idocToFile(temp, string(), theconfig, idoc)) {
        temp.reset(); // just in case.
      }
      if (temp) {
        fn = temp->filename();
        pvEdit->m_tmpfilename = fn;
      } else {
        pvEdit->m_tmpfilename.erase();
        fn.erase();
      }
    }

    if (!fn.empty()) {
      pvEdit->m_image = QImage(fn.c_str());
      if (!pvEdit->m_image.isNull())
        pvEdit->displayImage();
    }
  }

  // Position the pvEdit so that the first search term is visible
  // Enter document in document history
  LOGDEB("loadDocInCurrentTab: returning true\n");
  return true;
}

#ifndef PREVIEWTEXTEDIT_H
#define PREVIEWTEXTEDIT_H

// Always use a qtextbrowser for now, there is no compelling reason to
// switch to webkit here
#if 1 || defined(RESLIST_TEXTBROWSER)
#define PREVIEW_TEXTBROWSER
#endif
#ifdef PREVIEW_TEXTBROWSER
#include "preview_plaintorich.h"

#include <QTextBrowser>
#include <rcldoc.h>

#include <bits/shared_ptr.h>
#define PREVIEW_PARENTCLASS QTextBrowser
#else
#include <QtWebKit/QWebView>
#include <QWebEngineView>
#define PREVIEW_PARENTCLASS QWebView
#endif

class Preview;
class PreviewTextEdit : public PREVIEW_PARENTCLASS {
    Q_OBJECT
public:
    PreviewTextEdit(QWidget* parent, const char* name, Preview *pv);
    void moveToAnchor(const QString& name);
    enum DspType {PTE_DSPTXT, PTE_DSPFLDS, PTE_DSPIMG};

public slots:
    virtual void displayFields();
    virtual void displayText();
    virtual void displayImage();

    friend class Preview;


private:
    Preview *m_preview;
    std::shared_ptr<PlainToRichQtPreview> m_plaintorich;

    bool   m_dspflds;
    std::string m_url; // filename for this tab
    std::string m_ipath; // Internal doc path inside file
    int    m_docnum;  // Index of doc in db search results.

    // doc out of internfile (previous fields come from the index) with
    // main text erased (for space).
    Rcl::Doc m_fdoc;

    // The input doc out of the index/query list
    Rcl::Doc m_dbdoc;

    // Saved rich (or plain actually) text: the textedit seems to
    // sometimes (but not always) return its text stripped of tags, so
    // this is needed (for printing for example)
    QString  m_richtxt;
    Qt::TextFormat m_format;

    // Temporary file name (possibly, if displaying image). The
    // TempFile itself is kept inside main.cpp (because that's where
    // signal cleanup happens), but we use its name to ask for release
    // when the tab is closed.
    std::string m_tmpfilename;
    QImage m_image;
    DspType m_curdsp;
};
#endif // PREVIEWTEXTEDIT_H

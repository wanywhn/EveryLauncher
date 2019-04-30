#include "previewtextedit.h"
#include "log.h"

PreviewTextEdit::PreviewTextEdit(QWidget* parent, const char* nm, Preview *pv)
    : QTextBrowser(parent), m_preview(pv),
      m_plaintorich(new PlainToRichQtPreview()),
      m_dspflds(false), m_docnum(-1)
{
    setObjectName(nm);
    setOpenExternalLinks(false);
    setOpenLinks(false);
}


// Display main text
void PreviewTextEdit::displayText()
{
    LOGDEB1("PreviewTextEdit::displayText()\n");
    if (m_format == Qt::PlainText)
        setPlainText(m_richtxt);
    else
        setHtml(m_richtxt);
    m_curdsp = PTE_DSPTXT;
}

// Display field values
void PreviewTextEdit::displayFields()
{
    LOGDEB1("PreviewTextEdit::displayFields()\n");

    QString txt = "<html><head></head><body>\n";
    txt += "<b>" + QString::fromLocal8Bit(m_url.c_str());
    if (!m_ipath.empty())
        txt += "|" + QString::fromUtf8(m_ipath.c_str());
    txt += "</b><br><br>";
    txt += "<dl>\n";
    for (std::map<string,string>::const_iterator it = m_fdoc.meta.begin();
         it != m_fdoc.meta.end(); it++) {
        if (!it->second.empty())
            txt += "<dt>" + QString::fromUtf8(it->first.c_str()) + "</dt> "
                + "<dd>" + QString::fromUtf8(escapeHtml(it->second).c_str())
                + "</dd>\n";
    }
    txt += "</dl></body></html>";
    setHtml(txt);
    m_curdsp = PTE_DSPFLDS;
}

void PreviewTextEdit::displayImage()
{
    LOGDEB1("PreviewTextEdit::displayImage()\n");
    if (m_image.isNull())
        displayText();

    setPlainText("");
    if (m_image.width() > width() ||
        m_image.height() > height()) {
        m_image = m_image.scaled(width(), height(), Qt::KeepAspectRatio);
    }
    document()->addResource(QTextDocument::ImageResource, QUrl("image"),
                            m_image);
    QTextCursor cursor = textCursor();
    cursor.insertImage("image");
    m_curdsp = PTE_DSPIMG;
}

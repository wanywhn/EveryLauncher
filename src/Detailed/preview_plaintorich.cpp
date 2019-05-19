/* Copyright (C) 2014 J.F.Dockes
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <stdio.h>

#include <string>

#include <QString>
#include <QStringList>

#include "preview_plaintorich.h"

#include "plaintorich.h"
#include "log.h"
#include "guiutils.h"
#include "cancelcheck.h"

using namespace std;

PlainToRichQtPreview::PlainToRichQtPreview()
{
    clear();
}

void PlainToRichQtPreview::clear()
{
    m_curanchor = 1; 
    m_lastanchor = 0;
    m_groupanchors.clear();
    m_groupcuranchors.clear();
}

bool PlainToRichQtPreview::haveAnchors()
{
    return m_lastanchor != 0;
}

string  PlainToRichQtPreview::PlainToRichQtPreview::header()
{
    if (!m_inputhtml) {
        switch (PrefsPack::PP_PREWRAP) {
        case PrefsPack::PP_BR:
            m_eolbr = true;
            return "<qt><head><title></title></head><body>";
        case PrefsPack::PP_PRE:
            m_eolbr = false;
            return "<qt><head><title></title></head><body><pre>";
        case PrefsPack::PP_PREWRAP:
            m_eolbr = false;
            return "<qt><head><title></title></head><body>"
                "<pre style=\"white-space: pre-wrap\">";
        }
    }
    return cstr_null;
}

string PlainToRichQtPreview::startMatch(unsigned int grpidx)
{
    LOGDEB2("startMatch, grpidx "  << (grpidx) << "\n" );
    grpidx = m_hdata->grpsugidx[grpidx];
    LOGDEB2("startMatch, ugrpidx "  << (grpidx) << "\n" );
    m_groupanchors[grpidx].push_back(++m_lastanchor);
    m_groupcuranchors[grpidx] = 0; 
    return string("<span style='").
        append("color: blue").
        append("'>").
        append("<a name=\"").
        append(termAnchorName(m_lastanchor)).
        append("\">");
}

string  PlainToRichQtPreview::endMatch()
{
    return string("</a></span>");
}

string  PlainToRichQtPreview::termAnchorName(int i) const
{
    static const char *termAnchorNameBase = "TRM";
    char acname[sizeof(termAnchorNameBase) + 20];
    sprintf(acname, "%s%d", termAnchorNameBase, i);
    return string(acname);
}

string  PlainToRichQtPreview::startChunk()
{
    return "<pre>";
}

int  PlainToRichQtPreview::nextAnchorNum(int grpidx)
{
    LOGDEB2("nextAnchorNum: group "  << (grpidx) << "\n" );
    map<unsigned int, unsigned int>::iterator curit = 
        m_groupcuranchors.find(grpidx);
    map<unsigned int, vector<int> >::iterator vecit = 
        m_groupanchors.find(grpidx);
    if (grpidx == -1 || curit == m_groupcuranchors.end() ||
        vecit == m_groupanchors.end()) {
        if (m_curanchor >= m_lastanchor)
            m_curanchor = 1;
        else
            m_curanchor++;
    } else {
        if (curit->second >= vecit->second.size() -1)
            m_groupcuranchors[grpidx] = 0;
        else 
            m_groupcuranchors[grpidx]++;
        m_curanchor = vecit->second[m_groupcuranchors[grpidx]];
        LOGDEB2("nextAnchorNum: curanchor now "  << (m_curanchor) << "\n" );
    }
    return m_curanchor;
}

int  PlainToRichQtPreview::prevAnchorNum(int grpidx)
{
    map<unsigned int, unsigned int>::iterator curit = 
        m_groupcuranchors.find(grpidx);
    map<unsigned int, vector<int> >::iterator vecit = 
        m_groupanchors.find(grpidx);
    if (grpidx == -1 || curit == m_groupcuranchors.end() ||
        vecit == m_groupanchors.end()) {
        if (m_curanchor <= 1)
            m_curanchor = m_lastanchor;
        else
            m_curanchor--;
    } else {
        if (curit->second <= 0)
            m_groupcuranchors[grpidx] = vecit->second.size() -1;
        else 
            m_groupcuranchors[grpidx]--;
        m_curanchor = vecit->second[m_groupcuranchors[grpidx]];
    }
    return m_curanchor;
}

QString  PlainToRichQtPreview::curAnchorName() const
{
    return QString::fromUtf8(termAnchorName(m_curanchor).c_str());
}


ToRichThread::ToRichThread(const string &i, const HighlightData& hd,
                           std::shared_ptr<PlainToRichQtPreview> ptr,
                           QStringList& qrichlist,
                           QObject *parent)
    : QThread(parent), m_input(i), m_hdata(hd), m_ptr(ptr), m_output(qrichlist)
{
}

// Insert into editor by chunks so that the top becomes visible
// earlier for big texts. This provokes some artifacts (adds empty line),
// so we can't set it too low.
#define CHUNKL 500*1000

void ToRichThread::run()
{
    list<string> out;
    try {
        m_ptr->plaintorich(m_input, out, m_hdata, CHUNKL);
    } catch (CancelExcept) {
        return;
    }

    // Convert C++ string list to QString list
    for (list<string>::iterator it = out.begin(); 
         it != out.end(); it++) {
        m_output.push_back(QString::fromUtf8(it->c_str(), it->length()));
    }
}


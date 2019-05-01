/* Copyright (C) 2006 J.F.Dockes
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
#ifndef _PREVIEW_W_H_INCLUDED_
#define _PREVIEW_W_H_INCLUDED_

#include <stdio.h>
#include <memory>
#include <QComboBox>

#include "detailedtext.h"
#include "plaintorich.h"
#include "previewtextedit.h"
#include "rcldb.h"

class QTabWidget;
class QLabel;
class QPushButton;
class QCheckBox;
class Preview;
class PlainToRichQtPreview;
class QUrl;

class Preview : public DetailedW {
  Q_OBJECT;

public:
  Preview( )// Search terms etc. for highlighting
      : DetailedW(0) {
    init();
  }

  friend class PreviewTextEdit;

signals:
  void previewClosed(Preview *);
  void wordSelect(QString);
  void showNext(Preview *w, int sid, int docnum);
  void showPrev(Preview *w, int sid, int docnum);
  void previewExposed(Preview *w, int sid, int docnum);
  void printCurrentPreviewRequest();

private:
  // Identifier of search in main window. This is used to check that
  // we make sense when requesting the next document when browsing
  // successive search results in a tab.

  bool m_dynSearchActive{false};
  // Index value the search text comes from. -1 if text was edited
  int m_searchTextFromIndex{-1};

  bool m_canBeep{true};
  bool m_loading{false};
//  HighlightData m_hData;
  PreviewTextEdit *pvEdit;

  void init();
  virtual bool loadDocInCurrentTab(const Rcl::Doc &idoc, int dnm);

  // DetailedW interface
public:
  void showDoc(Rcl::Doc doc) override;
};

#endif /* _PREVIEW_W_H_INCLUDED_ */

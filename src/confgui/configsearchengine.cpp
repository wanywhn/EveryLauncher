#include "configsearchengine.h"

#include <QHeaderView>
#include <QSettings>
#include <QVBoxLayout>

ConfigSearchEngine::ConfigSearchEngine(QWidget *parent) : QWidget(parent) {

  init_ui();
}

void ConfigSearchEngine::addRow() {
  twidget->insertRow(twidget->rowCount());

  sett.beginGroup("SearchPrefix");
  QStringList sl;
  sl << "URL"
     << "Name";
  sett.setValue("kv", sl);

  sett.endGroup();
  init_data();
}

void ConfigSearchEngine::deleteRow() {
  auto curr = twidget->currentRow();
  sett.beginGroup("SearchPrefix");
  auto k = sett.childKeys().at(curr);
  sett.remove(k);
  sett.endGroup();
  twidget->removeRow(curr);
}

void ConfigSearchEngine::cellChanged(int r, int c) {
  sett.beginGroup("SearchPrefix");
  auto item = twidget->item(r, c)->data(Qt::DisplayRole).toString();
  if (c == 0) {
    // search short
    //!!FIXME get original key
    //! here use r to get in settings

    auto origianlK = sett.childKeys().at(r);
    auto sl = twidget->item(r, 1)->data(Qt::DisplayRole).toStringList();
    sett.remove(origianlK);
    //!! FIXME check if exists
    sett.setValue(item, sl);
  } else if (c == 1) {
    // just change the value
    auto k = sett.childKeys().at(r);
    auto sl = sett.value(k).toStringList();
    sl[1] = item;
    sett.setValue(k, sl);
  } else if (c == 2) {
    auto k = sett.childKeys().at(r);
    auto sl = sett.value(k).toStringList();
    sl[1] = item;
    sett.setValue(k, sl);
  }

  sett.endGroup();
}

void ConfigSearchEngine::init_ui() {
  auto mlayout = new QVBoxLayout();
  this->setLayout(mlayout);

  twidget = new QTableWidget();
  sett.beginGroup("SearchPrefix");
  twidget->setRowCount(sett.childKeys().size());
  twidget->setColumnCount(3);
  sett.endGroup();
  init_data();

  twidget->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
  twidget->horizontalHeader()->setStretchLastSection(true);
  twidget->verticalHeader()->setHidden(true);
  QStringList sl;
  sl << "快捷键"
     << "名称"
     << "网址";
  twidget->setHorizontalHeaderLabels(sl);
  mlayout->addWidget(twidget);

  btnAdd = new QPushButton("AddRow");
  connect(btnAdd, &QPushButton::clicked, this, &ConfigSearchEngine::addRow);
  btnDelete = new QPushButton("DeleteRow");
  connect(btnDelete, &QPushButton::clicked, this,
          &ConfigSearchEngine::deleteRow);

  mlayout->addWidget(btnAdd);
  mlayout->addWidget(btnDelete);
}

void ConfigSearchEngine::init_data() {
  twidget->disconnect();
  twidget->clearContents();
  sett.beginGroup("SearchPrefix");
  for (auto i = 0; i != sett.childKeys().size(); ++i) {
    auto k = sett.childKeys().at(i);
    // TODO give a name
    auto kitem = new QTableWidgetItem(k);
    auto sl = sett.value(k).toStringList();
    auto vitem = new QTableWidgetItem(sl.at(0));
    auto nitem = new QTableWidgetItem(sl.at(1));
    twidget->setItem(i, 0, kitem);
    twidget->setItem(i, 1, nitem);
    twidget->setItem(i, 2, vitem);
  }
  connect(twidget, &QTableWidget::cellChanged, this,
          &ConfigSearchEngine::cellChanged);
  sett.endGroup();
}

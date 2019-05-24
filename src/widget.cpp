#include <utility>
#include <QDebug>
#include <QMessageBox>
#include <QProgressDialog>
#include <QThread>
#include <QVBoxLayout>
#include <docseqdb.h>
#include <wasatorcl.h>

#include "widget.h"


MainWindow::MainWindow(QWidget *parent) : DMainWindow(parent) {
    this->restable = new ResWidget(this);
    this->searchLine = new SearchWidget(this);
    this->m_model = new UnitedModel(this);
    this->escKey = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    this->upKey = new QShortcut(QKeySequence(Qt::Key_Up), this);
    this->downkey = new QShortcut(QKeySequence(Qt::Key_Down), this);
    init_ui();
    init_conn();
}


void MainWindow::init_ui() {

    auto cw = new QWidget(this);
    this->setCentralWidget(cw);

    auto mvLayout = new QVBoxLayout();
    cw->setLayout(mvLayout);

    this->centralWidget()->setLayout(mvLayout);
    mvLayout->addWidget(searchLine);
    mvLayout->addWidget(restable);

    restable->setM_model(this->m_model);
}

void MainWindow::init_conn() {
    connect(this->searchLine, &SearchWidget::startSearch, m_model, &UnitedModel::startSearch);

    connect(this->searchLine, &SearchWidget::clearSearch, restable, &ResWidget::cleanSearch);

    connect(this->searchLine, &SearchWidget::returnPressed, this->restable, &ResWidget::returnPressed);

    connect(this->escKey, &QShortcut::activated, this->searchLine, &SearchWidget::clearAll);

    connect(this->upKey, &QShortcut::activated, this->restable, &ResWidget::currentMoveUp);

    connect(this->downkey, &QShortcut::activated, this->restable, &ResWidget::currentMoveDown);

}



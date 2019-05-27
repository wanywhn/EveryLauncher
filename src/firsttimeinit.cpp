#include "firsttimeinit.h"

#include <QDir>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>
#include <QStandardPaths>
#include <QVBoxLayout>

#include "config.h"


firstTimeInit::firstTimeInit(QWidget *parent) : DDialog(parent) {
    plusBtn = new DTextButton("添加", this);
    minusBtn = new DTextButton("删除", this);
    startBtn = new DTextButton("立即开始索引", this);

    listview = new DListView(this);
    model = new QStringListModel(this);
    lists.clear();
//  model->setStringList(
    auto appPaths = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation);
    for (const auto &item:appPaths) {
        if (QFile::exists(item)) {
            lists << item;
        }
    }
    model->setStringList(lists);

    listview->setModel(model);
    listview->setSelectionMode(QAbstractItemView::SingleSelection);

    init_ui();
    init_conn();
}

void firstTimeInit::init_ui() {
    auto vlayout = this->layout();
    vlayout->setMargin(8);

    auto label = new QLabel(this);
    label->setText(
            "第一次运行需要您设置一下需要索引的目录，请点击下面的加号与减号配置。<br>"
            "设置之后您随时可以前往系统托盘处进入设置来更改您的配置<br>");
    vlayout->addWidget(label);
    vlayout->addWidget(listview);

    auto btnbox = new QVBoxLayout;
    btnbox->addWidget(plusBtn);
    btnbox->addWidget(minusBtn);

    auto hlayout = new QHBoxLayout;
    hlayout->addLayout(btnbox);
    hlayout->addWidget(listview);

    vlayout->addItem(hlayout);

    label = new QLabel(this);
    label->setText(
            "已经预先定义了几个目录,这是用来检索已安装程序的A，强烈建议您保留。");
    vlayout->addWidget(label);

    auto startLayout = new QHBoxLayout;
    startLayout->addStretch();
    startLayout->addWidget(startBtn);
//  startLayout->addWidget(cp);
//  cp->hide();
    startLayout->addStretch();

    vlayout->addItem(startLayout);
}

void firstTimeInit::init_conn() {

    connect(plusBtn, &DTextButton::clicked, [this]() {
        auto path = QFileDialog::getExistingDirectory(this, tr("请选择需要监控的目录"), QDir::homePath());
        if (!path.isEmpty())
            listview->addItem(path);
    });
    connect(minusBtn, &DTextButton::clicked,
            [this]() { listview->removeItem(listview->currentIndex().row()); });
    connect(startBtn, &DTextButton::clicked, this, &firstTimeInit::setTopdirsAndIndex);
}

void firstTimeInit::setTopdirsAndIndex() {
    qDebug()<<"start set topdirs";
    if (listview->count() == 0) {
        QMessageBox::warning(this, tr("目录为空"), tr("警告，监控目录为空"));
        return;
    }
    auto m_conf = theconfig->cloneMainConfig();
    m_conf->holdWrites(true);
    std::vector<std::string> ssl;
    ssl.reserve(model->rowCount());
    std::string val;
    for (auto item : model->stringList()) {
        ssl.emplace_back(item.toStdString());
    }
    stringsToString(ssl, val);
    m_conf->set("topdirs", val);
    m_conf->holdWrites(false);
    delete m_conf;
    m_conf = nullptr;
    theconfig->updateMainConfig();

    QProcess *p = new QProcess(this);
    QStringList args;
    args << "-c";
    args << QString::fromStdString(theconfig->getConfDir());

//    args << "-i";
    args << "-z";
//    args << model->stringList();
    cp = new QProgressDialog(this);
    connect(p, &QProcess::readyRead, [this, p]() {
        QTextStream ts(p);

        QRegularExpression rxp(R"(docid (\d+) (?:added|updated) \[(.*)\|\]$)");
        while (!ts.atEnd()) {
            auto l = ts.readLine();
            auto matches = rxp.match(l);
            if (!matches.hasMatch()) {
                continue;
            }
            QString text = "正在处理第" + matches.captured(1) + "个文件:" + matches.captured(2);
            cp->setLabelText(text);
        }
    });
    connect(p, QOverload<int>::of(&QProcess::finished), [this]() {
        cp->hide();
        this->accept();
    });
    connect(p, &QProcess::errorOccurred, [this]() {
        cp->hide();
    });
    p->setProcessChannelMode(QProcess::MergedChannels);
    cp->setWindowTitle(tr("正在索引，请稍后。"));
    cp->setRange(0, 0);
    cp->show();
    p->start("recollindex", args);


}

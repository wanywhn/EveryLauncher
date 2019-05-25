/* Copyright (C) 2007 J.F.Dockes
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

#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QListWidget>
#include <QVBoxLayout>
#include <qcheckbox.h>
#include <qglobal.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qtimer.h>
#include <qwidget.h>

#include <confgui/configsearchengine.h>
#include <dbusproxy.h>
#include <functional>
#include <list>
#include <set>
#include <string>
using std::list;
using std::set;
using std::string;

#include "confgui.h"
//#include "recoll.h"
#include "confguiindex.h"
#include "execmd.h"
#include "log.h"
#include "rclconfig.h"
#include "rcldb.h"
#include "smallut.h"
#include "ConfigModelEngin.h"

namespace ELconfgui {
static const int spacing = 3;
static const int margin = 3;

/**
 * A Gui-to-Data link class for ConfTree
 * Has a subkey pointer member which makes it easy to change the
 * current subkey for a number at a time.
 */
class ConfLinkRclRep : public ConfLinkRep {
public:
  ConfLinkRclRep(ConfNull *conf, const string &nm, string *sk = 0)
      : m_conf(conf), m_nm(nm), m_sk(sk) /* KEEP THE POINTER, shared data */
  {}
  virtual ~ConfLinkRclRep() {}

  virtual bool set(const string &val) {
    if (!m_conf)
      return false;
    LOGDEB1("Setting [" << m_nm << "] value to [" << val << "]\n");
    bool ret = m_conf->set(m_nm, val, getSk());
    if (!ret)
      LOGERR("Value set failed\n");
    return ret;
  }
  virtual bool get(string &val) {
    if (!m_conf)
      return false;
    bool ret = m_conf->get(m_nm, val, getSk());
    LOGDEB1("ConfLinkRcl::get: [" << m_nm << "] sk [" << getSk() << "] -> ["
                                  << (ret ? val : "no value") << "]\n");
    return ret;
  }

private:
  string getSk() { return m_sk ? *m_sk : string(); }
  ConfNull *m_conf;
  const string m_nm;
  const string *m_sk;
};

typedef std::function<vector<string>()> RclConfVecValueGetter;

/* Special link for skippedNames and noContentSuffixes which are
   computed as set differences */
class ConfLinkPlusMinus : public ConfLinkRep {
public:
  ConfLinkPlusMinus(RclConfig *rclconf, ConfNull *conf, const string &basename,
                    RclConfVecValueGetter getter, string *sk = 0)
      : m_rclconf(rclconf), m_conf(conf), m_basename(basename),
        m_getter(getter), m_sk(sk) /* KEEP THE POINTER, shared data */
  {}
  virtual ~ConfLinkPlusMinus() {}

  virtual bool set(const string &snval) {
    if (!m_conf || !m_rclconf)
      return false;

    string sbase;
    m_conf->get(m_basename, sbase, getSk());
    std::set<string> nval;
    stringToStrings(snval, nval);
    string splus, sminus;
    RclConfig::setPlusMinus(sbase, nval, splus, sminus);
    LOGDEB1("ConfLinkPlusMinus: base [" << sbase << "] nvalue [" << snval
                                        << "] splus [" << splus << "] sminus ["
                                        << sminus << "]\n");
    if (!m_conf->set(m_basename + "-", sminus, getSk())) {
      return false;
    }
    if (!m_conf->set(m_basename + "+", splus, getSk())) {
      return false;
    }
    return true;
  }

  virtual bool get(string &val) {
    if (!m_conf || !m_rclconf)
      return false;

    m_rclconf->setKeyDir(getSk());
    vector<string> vval = m_getter();
    val = stringsToString(vval);
    LOGDEB1("ConfLinkPlusMinus: " << m_basename << " -> " << val << std::endl);
    return true;
  }

private:
  string getSk() { return m_sk ? *m_sk : string(); }

  RclConfig *m_rclconf;
  ConfNull *m_conf;
  string m_basename;
  RclConfVecValueGetter m_getter;
  const string *m_sk;
};

ConfIndexW::ConfIndexW(QWidget *parent, RclConfig *config)
    : DDialog(parent), m_rclconf(config) {
  QString title("EveryLauncher - Index Settings : ");
  title += QString::fromLocal8Bit(config->getConfDir().c_str());
  setWindowTitle(title);
  tabWidget = new QTabWidget(this);
  QString styleSheet="QTabWidget::pane{\
                      border:0px solid black;\
                     }\
                    ";
  tabWidget->setStyleSheet(styleSheet);
  reloadPanels();

  buttonBox =
      new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

  auto cw = new QWidget(this);
  this->addContent(cw);
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(tabWidget);
  mainLayout->addWidget(buttonBox);
  cw->setLayout(mainLayout);

  resize(QSize(600, 450).expandedTo(minimumSizeHint()));

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(acceptChanges()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(rejectChanges()));
}

void ConfIndexW::acceptChanges() {
  LOGDEB("ConfIndexW::acceptChanges()\n");
  if (!m_conf) {
    LOGERR("ConfIndexW::acceptChanges: no config\n");
    return;
  }
  // Let the changes to disk
  if (!m_conf->holdWrites(false)) {
    QMessageBox::critical(0, "Recoll", tr("Can't write configuration file"));
  }
  // Delete local copy and update the main one from the file
  delete m_conf;
  m_conf = 0;
  m_rclconf->updateMainConfig();

  hide();
}

void ConfIndexW::rejectChanges() {
  LOGDEB("ConfIndexW::rejectChanges()\n");
  // Discard local changes.
  delete m_conf;
  m_conf = 0;
  hide();
}

void ConfIndexW::reloadPanels() {
  if ((m_conf = m_rclconf->cloneMainConfig()) == 0)
    return;
  m_conf->holdWrites(true);
  m_widgets.clear();
  tabWidget->clear();

  QWidget *w = new ConfTopPanelW(this, m_conf);
  m_widgets.push_back(w);
  tabWidget->addTab(w, QObject::tr("全局参数"));

  w = new ConfSubPanelW(this, m_conf, m_rclconf);
  m_widgets.push_back(w);
  tabWidget->addTab(w, QObject::tr("局部参数"));

  w=new ConfigSearchEngine(this);
  m_widgets.push_back(w);
  tabWidget->addTab(w,QObject::tr("Search Engine"));

  w=new ConfigModelEngine();
  m_widgets.push_back(w);
  tabWidget->addTab(w,QObject::tr("数据来源"));

}

ConfTopPanelW::ConfTopPanelW(QWidget *parent, ConfNull *config)
    : QWidget(parent) {
  QWidget *w = 0;

  QGridLayout *gl1 = new QGridLayout(this);

  int gridrow = 0;
  w = new ConfParamDNLW(this, ConfLink(new ConfLinkRclRep(config, "topdirs")),
                        tr("监控目录"),
                        tr("进行索引时将递归索引该目录及子目录，"
                           "默认：家目录"));
  setSzPol(w, QSizePolicy::Preferred, QSizePolicy::Preferred, 1, 3);
  gl1->addWidget(w, gridrow++, 0, 1, 2);

  ConfParamSLW *eskp = new ConfParamSLW(
      this, ConfLink(new ConfLinkRclRep(config, "skippedPaths")),
      tr("略过路径"), tr("当索引时这些目录将被略过 "));

  eskp->setFsEncoding(true);
  setSzPol(eskp, QSizePolicy::Preferred, QSizePolicy::Preferred, 1, 3);
  gl1->addWidget(eskp, gridrow++, 0, 1, 2);

  w=new ConfParamIntW(this,
                      ConfLink(new ConfLinkRclRep(config,"monitorIndexInterval")),
                      tr("文件监控报告间隔"),
                      tr("设置多长时间文件变更监控汇报一次，以进行索引，默认600秒"),
                      1,99999);

  gl1->addWidget(w, gridrow++, 1);

  w = new ConfParamIntW(this,
                        ConfLink(new ConfLinkRclRep(config, "idxflushmb")),
                        tr("索引缓存写入配额"),
                        tr("这个值控制着索引器将索引缓存写入磁盘的触发值"
                           "默认是10MB "),
                        0, 1000);
  gl1->addWidget(w, gridrow++, 1);
}

ConfSubPanelW::ConfSubPanelW(QWidget *parent, ConfNull *config,
                             RclConfig *rclconf)
    : QWidget(parent), m_config(config) {
  QVBoxLayout *vboxLayout = new QVBoxLayout(this);
  vboxLayout->setSpacing(spacing);
  vboxLayout->setMargin(margin);

  m_subdirs = new ConfParamDNLW(
      this, ConfLink(new ConfLinkNullRep()), QObject::tr("自定义子目录"),
      QObject::tr("这里是索引目录下面的子目录。</br> "
                  "我们可以在这里针对子目录进行一些详细索引配置。 </br>"
                  "默认空目录"));
  m_subdirs->getListBox()->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(m_subdirs->getListBox(),
          SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
          this, SLOT(subDirChanged(QListWidgetItem *, QListWidgetItem *)));
  connect(m_subdirs, SIGNAL(entryDeleted(QString)), this,
          SLOT(subDirDeleted(QString)));

  // We only retrieve the subkeys from the user's config (shallow),
  // no use to confuse the user by showing the subtrees which are
  // customized in the system config like .thunderbird or
  // .purple. This doesn't prevent them to add and customize them
  // further.
  vector<string> allkeydirs = config->getSubKeys(true);

  QStringList qls;
  for (vector<string>::const_iterator it = allkeydirs.begin();
       it != allkeydirs.end(); it++) {
    qls.push_back(QString::fromUtf8(it->c_str()));
  }
  m_subdirs->getListBox()->insertItems(0, qls);
  vboxLayout->addWidget(m_subdirs);

  QFrame *line2 = new QFrame(this);
  line2->setFrameShape(QFrame::HLine);
  line2->setFrameShadow(QFrame::Sunken);
  vboxLayout->addWidget(line2);

  QLabel *explain = new QLabel(this);
  explain->setText(QObject::tr("<i>如果上面的列表中选定了某一项</br>"
                               "那么下面的参数就是针对那个子目录进行配置的</br>"
                               "否则就是针对根目录配置。"
                               "你可以使用+/-按钮来增加删除文件夹。"));
  vboxLayout->addWidget(explain);

  m_groupbox = new QGroupBox(this);
  setSzPol(m_groupbox, QSizePolicy::Preferred, QSizePolicy::Preferred, 1, 3);

  QGridLayout *gl1 = new QGridLayout(m_groupbox);
  int gridy = 0;

  ConfParamSLW *eskn = new ConfParamSLW(
      m_groupbox,
      ConfLink(new ConfLinkPlusMinus(
          rclconf, config, "skippedNames",
          std::bind(&RclConfig::getSkippedNames, rclconf), &m_sk)),
      QObject::tr("将被略过的名称"),
      QObject::tr("这是不想被索引的文件或文件夹的模式 "));

  eskn->setFsEncoding(true);
  m_widgets.push_back(eskn);
  gl1->addWidget(eskn, gridy, 0);

  vector<string> amimes = rclconf->getAllMimeTypes();
  QStringList amimesq;
  for (vector<string>::const_iterator it = amimes.begin(); it != amimes.end();
       it++) {
    amimesq.push_back(QString::fromUtf8(it->c_str()));
  }

  ConfParamCSLW *eincm = new ConfParamCSLW(
      m_groupbox,
      ConfLink(new ConfLinkRclRep(config, "indexedmimetypes", &m_sk)),
      tr("只索引这些类型"),
      tr("索引程序将会之所因这些类型的文件，默认不启用。 "), amimesq);
  m_widgets.push_back(eincm);
  gl1->addWidget(eincm, gridy++, 1);

  ConfParamCSLW *eexcm = new ConfParamCSLW(
      m_groupbox,
      ConfLink(new ConfLinkRclRep(config, "excludedmimetypes", &m_sk)),
      tr("类型排除列表"), tr("这些类型的文件将不会被索引"), amimesq);
  m_widgets.push_back(eexcm);
  gl1->addWidget(eexcm, gridy, 0);

  ConfParamSLW *encs = new ConfParamSLW(
      m_groupbox,
      ConfLink(new ConfLinkPlusMinus(
          rclconf, config, "noContentSuffixes",
          std::bind(&RclConfig::getStopSuffixes, rclconf), &m_sk)),
      QObject::tr("忽略文件类型的后缀"),
      QObject::tr("这些后缀的文件将只进行文件名索引"));
  encs->setFsEncoding(true);
  m_widgets.push_back(encs);
  gl1->addWidget(encs, gridy++, 1);

  /*
  vector<string> args;
  args.push_back("-l");
  ExecCmd ex;
  string icout;
  string cmd = "iconv";
  int status = ex.doexec(cmd, args, 0, &icout);
  if (status) {
    LOGERR("Can't get list of charsets from 'iconv -l'");
  }
  icout = neutchars(icout, ",");
  list<string> ccsets;
  stringToStrings(icout, ccsets);
  QStringList charsets;
  charsets.push_back("");
  for (list<string>::const_iterator it = ccsets.begin(); it != ccsets.end();
       it++) {
    charsets.push_back(QString::fromUtf8(it->c_str()));
  }

  ConfParamCStrW *e21 = new ConfParamCStrW(
      m_groupbox, ConfLink(new ConfLinkRclRep(config, "defaultcharset", &m_sk)),
      QObject::tr("Default<br>character set"),
      QObject::tr("Character set used for reading files "
                  "which do not identify the character set "
                  "internally, for example pure text files.<br>"
                  "The default value is empty, "
                  "and the value from the NLS environnement is used."),
      charsets);
  m_widgets.push_back(e21);
  gl1->addWidget(e21, gridy++, 0);
  */

  ConfParamBoolW *e3 = new ConfParamBoolW(
      m_groupbox, ConfLink(new ConfLinkRclRep(config, "followLinks", &m_sk)),
      QObject::tr("跟踪连接符号"),
      QObject::tr("当索引的时候，跟踪到符号链接里面去索引</br>"
                  "默认不开启"));
  m_widgets.push_back(e3);
  gl1->addWidget(e3, gridy, 0);

  ConfParamBoolW *eafln = new ConfParamBoolW(
      m_groupbox,
      ConfLink(new ConfLinkRclRep(config, "indexallfilenames", &m_sk)),
      QObject::tr("所有所有的文件名"),
      QObject::tr("对于目录中的所有文件都至少进行文件名称索引"));
  m_widgets.push_back(eafln);
  gl1->addWidget(eafln, gridy++, 1);

  ConfParamIntW *ezfmaxkbs = new ConfParamIntW(
      m_groupbox,
      ConfLink(new ConfLinkRclRep(config, "compressedfilemaxkbs", &m_sk)),
      tr("压缩文件最大处理大小"),
      tr("超过此文件大小的压缩文件将不会被索引，单位 KB </br>"
         "-1表示不限制，0表示不索引压缩包内部"),
      -1, 1000000, -1);
  m_widgets.push_back(ezfmaxkbs);
  gl1->addWidget(ezfmaxkbs, gridy, 0);

  ConfParamIntW *etxtmaxmbs = new ConfParamIntW(
      m_groupbox, ConfLink(new ConfLinkRclRep(config, "textfilemaxmbs", &m_sk)),
      tr("文档最大大小"),
      tr("超过此文件大小的文档将不会被索引，单位 MB"
         "-1表示不限制"),
      -1, 1000000);
  m_widgets.push_back(etxtmaxmbs);
  gl1->addWidget(etxtmaxmbs, gridy++, 1);

  /*
  ConfParamIntW *etxtpagekbs = new ConfParamIntW(
      m_groupbox,
      ConfLink(new ConfLinkRclRep(config, "textfilepagekbs", &m_sk)),
      tr("Text file page size (KB)"),
      tr("If this value is set (not equal to -1), text "
         "files will be split in chunks of this size for "
         "indexing.\nThis will help searching very big text "
         " files (ie: log files)."),
      -1, 1000000);
  m_widgets.push_back(etxtpagekbs);
  gl1->addWidget(etxtpagekbs, gridy, 0);
  */

  ConfParamIntW *efiltmaxsecs = new ConfParamIntW(
      m_groupbox,
      ConfLink(new ConfLinkRclRep(config, "filtermaxseconds", &m_sk)),
      tr("文档处理器最大运行时间，单位秒"),
      tr("外部文档处理器的最大运行时间，"
         "超过这个运行时间将被停止。很少会达到这个情况，通常是是可能死循环了 </br>"
         "-1表示无限制"),
      -1, 10000);
  m_widgets.push_back(efiltmaxsecs);
  gl1->addWidget(efiltmaxsecs, gridy++, 1);

  vboxLayout->addWidget(m_groupbox);
  subDirChanged(0, 0);
}

void ConfSubPanelW::reloadAll() {
  for (list<ConfParamW *>::iterator it = m_widgets.begin();
       it != m_widgets.end(); it++) {
    (*it)->loadValue();
  }
}

void ConfSubPanelW::subDirChanged(QListWidgetItem *current, QListWidgetItem *) {
  LOGDEB("ConfSubPanelW::subDirChanged\n");

  if (current == 0 || current->text() == "") {
    m_sk = "";
    m_groupbox->setTitle(tr("全局设置"));
  } else {
    m_sk = (const char *)current->text().toUtf8();
    m_groupbox->setTitle(current->text());
  }
  LOGDEB("ConfSubPanelW::subDirChanged: now [" << (m_sk) << "]\n");
  reloadAll();
}

void ConfSubPanelW::subDirDeleted(QString sbd) {
  LOGDEB("ConfSubPanelW::subDirDeleted(" << ((const char *)sbd.toUtf8())
                                         << ")\n");
  if (sbd == "") {
    // Can't do this, have to reinsert it
    QTimer::singleShot(0, this, SLOT(restoreEmpty()));
    return;
  }
  // Have to delete all entries for submap
  m_config->eraseKey((const char *)sbd.toUtf8());
}

void ConfSubPanelW::restoreEmpty() {
  LOGDEB("ConfSubPanelW::restoreEmpty()\n");
  m_subdirs->getListBox()->insertItem(0, "");
}

} // Namespace confgui

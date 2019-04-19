import os
import random
import sys

from PySide2 import QtCore
from PySide2.QtCore import QAbstractListModel, QTimer, Qt, QByteArray, Property, Signal, Slot, QModelIndex
from PySide2.QtGui import QKeySequence, QIcon
from PySide2.QtWidgets import QMainWindow, QShortcut, QFileDialog
from xdg.Exceptions import ParsingError, DuplicateGroupError, DuplicateKeyError

from launcher.config import CONFIG_DIR
from xdg.DesktopEntry import DesktopEntry
from xdg.IconTheme import getIconPath
import subprocess

try:
    from recoll import recoll
    from recoll import rclextract

    hasextract = True
except:
    import recoll

    hasextract = False

from getopt import getopt


####################
# Highlighting methods. Just for showing the groups usage, we add the
# original string for the match to the highlighted text. I don't think
# you'd want to do this in a real app, but maybe some kind of tooltip?
class HlMeths:
    def __init__(self, groups):
        self.groups = groups

    def startMatch(self, idx):
        ugroup = " ".join(self.groups[idx][0])
        return '<font size="tiny">' + ugroup + '</font><font color="blue">'

    def endMatch(self):
        return '</font>'


############
# Data extraction. The 2 following methods use the extractor module
# and get the data from the original document
#
# Extract and return document text (in text or html format, indicated
# by newdoc.mimetype)
# def textextract(doc):
#     extractor = rclextract.Extractor(doc)
#     newdoc = extractor.textextract(doc.ipath)
#     return newdoc


# Extract document in original format (ie: application/msword) and
# save it to a file. This only works if ipath is not null (else just
# use the url !)
def extractofile(doc, outfilename=""):
    extractor = rclextract.Extractor(doc)
    outfilename = extractor.idoctofile(doc.ipath, doc.mimetype, ofilename=outfilename)
    return outfilename


def getDesktopFile(url):
    #TODO nfs??
    url=str(url[7:])
    desktop=DesktopEntry()
    try:
        desktop.parse(url)
    except (ParsingError,DuplicateGroupError,DuplicateKeyError) as e:
        print(e)
        return None
    return desktop

#########
# RecollQuery wraps a recoll.query object in a Qt model
class recollQueryModel(QtCore.QAbstractListModel):
    Role_FILE_NAME = Qt.UserRole + 1
    Role_LOCATION = Qt.UserRole + 2
    Role_FILE_STATUS = Qt.UserRole + 3
    Role_FILE_SIMPLE_CONTENT = Qt.UserRole + 4
    Role_TYPE = Qt.UserRole + 5
    Role_FILE_ICON=Qt.UserRole+6
    Role_FILE_EXEC=Qt.UserRole+7
    Role_DESKTOP_NAME=Qt.UserRole+8
    Role_DESKTOP_DESC=Qt.UserRole+9


    @Signal
    def queryTextChanged(self):
        pass

    @Slot(int)
    def openFile(self,idx):

    ##FIXME open desktop file
        item=str(self.getdoc(idx)["url"])
        if item.endswith("desktop"):
            desktop=getDesktopFile(item)
            if  desktop is None:
                return
            print("ok:",desktop.getExec())
            u=desktop.getExec().split(" ")[0]
            if u.endswith("\""):
                u=u[:-1]
            if u.startswith("\""):
                u=u[1:]
            subprocess.Popen(u)
        else:
            subprocess.Popen(("xdg-open",item))



    @Slot(str)
    def setQueryText(self, val):
        self.set_queryText(val)

    def set_query_text(self, val):
        if (self.qtext != val):
            self.qtext = val
            self.startQuery()
            self.queryTextChanged.emit()

    def get_query_text(self):
        return self.qtext

    # once set quertText ,the query will exec
    queryText = Property(str, get_query_text, set_query_text, notify=queryTextChanged)

    def __init__(self, cfgDir, extra_dbs):
        QtCore.QAbstractListModel.__init__(self)
        self.totres = -1
        self.query = None
        self.qtext = ""
        self.searchResults = []
        self.pagelen = 6
        self.attrs = ("filename", "title", "mtime", "url", "ipath")
        # TODO read from config and prepare db

        # TODO confdir = CONFIG_DIR
        confdir = cfgDir
        extra_dbs = extra_dbs
        # Snippet params
        maxchars = 300
        contextwords = 6

        self.db = recoll.connect(confdir=confdir, extra_dbs=extra_dbs)
        self.db.setAbstractParams(maxchars=maxchars, contextwords=contextwords)

    def roleNames(self):
        roles = {
            recollQueryModel.Role_FILE_NAME: QByteArray(b'fileName'),
            recollQueryModel.Role_LOCATION: QByteArray(b'fileLocation'),
            recollQueryModel.Role_FILE_SIMPLE_CONTENT: QByteArray(b'fileSimpleContent'),
            recollQueryModel.Role_FILE_STATUS: QByteArray(b'fileStatus'),
            recollQueryModel.Role_FILE_ICON:QByteArray(b'fileIcon'),
            recollQueryModel.Role_FILE_EXEC:QByteArray(b'fileExec'),
            recollQueryModel.Role_DESKTOP_NAME:QByteArray(b'desktopName'),
            recollQueryModel.Role_DESKTOP_DESC:QByteArray(b'desktopDesc'),
        }
        return roles

    def rowCount(self, parent):
        ret = len(self.searchResults)
        # print("RecollQuery.rowCount(): %d"% ret)
        return ret

    # def columnCount(self, parent):
    #     print("RecollQuery.columnCount()")
    # if parent.isValid():
    #     return 0
    # else:
    #     return len(self.attrs)

    def setquery(self, db, q, sortfield="", ascending=True):
        """Parse and execute query on open db"""
        if q.strip() == "":
            self.beginResetModel()
            self.searchResults.clear()
            self.endResetModel()

            return
        self.query = db.query()
        if sortfield:
            self.query.sortby(sortfield, ascending)
        # Parse/run input query string
        self.totres = self.query.execute(q)
        # print("total res:",self.totres)
        self.qtext = q
        self.db = db
        self.beginResetModel()
        self.searchResults = []
        self.endResetModel()
        if self.canFetchMore(None):
            self.fetchMore(None)

    def getdoc(self, index):
        idx=0;
        if isinstance(index,int):
            idx=index
        elif isinstance(index,QModelIndex):
            idx=index.row()

        if idx < len(self.searchResults):
            return self.searchResults[idx]
        else:
            return None

    @Slot(int)
    def textextract(self, index):
        doc = self.searchResults[index]
        extractor = rclextract.Extractor(doc)
        newdoc = extractor.textextract(doc.ipath)
        print(newdoc.text)

    def sort(self, col, order):
        # print("sort %s %s", (col, order))
        self.setquery(self.db, self.qtext, sortfield=self.attrs[col],
                      ascending=order)

    # def headerData(self, idx, orient, role):
    #     if orient == QtCore.Qt.Horizontal and role == QtCore.Qt.DisplayRole:
    #         return self.attrs[idx]
    #     return None

    def data(self, index, role):
        # print("RecollQuery.data: row %d, role: %s" % (index.row(),role))
        if not index.isValid():
            return None

        if index.row() < 0:
            return None
        if index.row() >= len(self.searchResults):
            return None

        d = self.getdoc(index)
        # d = self.searchResults[index.row()]
        if role == recollQueryModel.Role_FILE_NAME:
            return d['filename']
        elif role == recollQueryModel.Role_LOCATION:
            return d['url']
        elif role == recollQueryModel.Role_FILE_SIMPLE_CONTENT:
            # return d['abstract']
            if d is None:
                print("No Doc")
                return
            groups = self.query.getgroups()
            # meths=HlMeths(groups)
            # tabs=self.query.makedocabstract(d,methods=meths)
            tabs = self.query.makedocabstract(d)
            tabs = self.query.highlight(tabs)
            return tabs


        elif role == recollQueryModel.Role_FILE_ICON:
            # icon=d['icon']
            desktop=getDesktopFile(d['url'])
            if desktop is None:
                return ""
            icon=desktop.getIcon()
            path=getIconPath(icon)

            print("path:",path)
            # print("path:",path)
            return path

        elif role==recollQueryModel.Role_FILE_EXEC:
            desktop=getDesktopFile(d['url'])
            if desktop is None:
                return ""
            return desktop.getExec().split(" ")[0]
        elif role==recollQueryModel.Role_DESKTOP_NAME:
            desktop=getDesktopFile(d['url'])
            if desktop is None:
                return ""
            return desktop.getName()
        elif role==recollQueryModel.Role_DESKTOP_DESC:
            desktop=getDesktopFile(d['url'])
            if desktop is None:
                return ""
            desc=desktop.getComment()
            if len(desc.strip())==0:
                desc=desktop.getGenericName()
            return desc



        return ""

    def canFetchMore(self, parent):
        # print("RecollQuery.canFetchMore:", self.searchResults," ",self.totres)
        if len(self.searchResults) < self.totres:
            return True
        else:
            return False

    def fetchMore(self, parent):
        # print("RecollQuery.fetchMore:")
        num_to_fetch = min(self.pagelen, self.totres - len(self.searchResults))
        titem=[]
        for count in range(num_to_fetch):
            try:
                item=self.query.fetchone()
                url=str(item['url'][7:])
                if url.endswith("desktop"):
                    desktop=DesktopEntry()
                    try:
                        desktop.parse(url)
                    except (ParsingError,DuplicateGroupError,DuplicateKeyError)as e:
                        print(e)
                    if desktop.getNoDisplay():
                        continue
                    if len(desktop.getOnlyShowIn())!=0:
                        continue


                titem.append(item)
            except:
                break
        self.beginInsertRows(QtCore.QModelIndex(), len(self.searchResults),
                             len(self.searchResults) + len(titem)- 1)
        self.searchResults.extend(titem)
        self.endInsertRows()

    def startQuery(self):
        self.setquery(self.db, self.qtext)


###
#  UI interaction code
class RclGui_Main(QMainWindow):
    def __init__(self, db, parent=None):
        scq = QShortcut(QKeySequence("Ctrl+Q"), self)
        scq.activated.connect(self.onexit)
        header = self.ui.resTable.horizontalHeader()
        header.setSortIndicatorShown(True)
        header.setSortIndicator(-1, QtCore.Qt.AscendingOrder)
        self.ui.resTable.setSortingEnabled(True)

    def on_resTable_clicked(self, index):
        doc = self.qmodel.getdoc(index)
        self.currentindex = index
        self.currentdoc = doc
        if doc is None:
            print("NO DoC")
            return
        query = self.qmodel.query
        groups = query.getgroups()
        meths = HlMeths(groups)
        abs = query.makedocabstract(doc, methods=meths)
        self.ui.resDetail.setText(abs)
        if hasextract:
            ipath = doc.get('ipath')
            # print("ipath[%s]" % ipath)
            self.ui.previewPB.setEnabled(True)
            if ipath:
                self.ui.savePB.setEnabled(True)
            else:
                self.ui.savePB.setEnabled(False)

    def on_previewPB_clicked(self):
        print("on_previewPB_clicked(self)")
        # newdoc = textextract(self.currentdoc)
        newdoc=self.currentdoc
        query = self.qmodel.query;
        groups = query.getgroups()
        meths = HlMeths(groups)
        # print("newdoc.mimetype:", newdoc.mimetype)
        if newdoc.mimetype == 'text/html':
            ishtml = True
        else:
            ishtml = False
        text = '<qt><head></head><body>' + \
               query.highlight(newdoc.text,
                               methods=meths,
                               ishtml=ishtml,
                               eolbr=True)
        text += '</body></qt>'
        self.ui.resDetail.setText(text)

    def on_savePB_clicked(self):
        doc = self.currentdoc
        ipath = doc.ipath
        if not ipath:
            return
        fn = QFileDialog.getSaveFileName(self)
        if fn:
            docitems = doc.items()
            fn = extractofile(doc, fn[0])
            print("Saved as %s" % fn)
        else:
            print("Canceled", file=sys.stderr)

    def onexit(self):
        self.close()

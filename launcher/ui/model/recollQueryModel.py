import random
import sys

from PySide2 import QtCore
from PySide2.QtCore import QAbstractListModel, QTimer, Qt, QByteArray, Property, Signal, Slot, QModelIndex
from PySide2.QtGui import QKeySequence
from PySide2.QtWidgets import QMainWindow, QShortcut, QFileDialog

from launcher.config import CONFIG_DIR

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
def textextract(doc):
    extractor = rclextract.Extractor(doc)
    newdoc = extractor.textextract(doc.ipath)
    return newdoc


# Extract document in original format (ie: application/msword) and
# save it to a file. This only works if ipath is not null (else just
# use the url !)
def extractofile(doc, outfilename=""):
    extractor = rclextract.Extractor(doc)
    outfilename = extractor.idoctofile(doc.ipath, doc.mimetype, \
                                       ofilename=outfilename)
    return outfilename


#########
# RecollQuery wraps a recoll.query object in a Qt model
class recollQueryModel(QtCore.QAbstractListModel):
    Role_FILE_NAME = Qt.UserRole + 1
    Role_LOCATION = Qt.UserRole + 2
    Role_FILE_STATUS = Qt.UserRole + 3
    Role_FILE_SIMPLE_CONTENT = Qt.UserRole + 4
    Role_TYPE=Qt.UserRole+5

    @Signal
    def queryTextChanged(self):
        pass

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

    def __init__(self):
        QtCore.QAbstractListModel.__init__(self)
        self.totres = -1
        self.query = None
        self.qtext = ""
        self.searchResults = []
        self.pagelen = 10
        self.attrs = ("filename", "title", "mtime", "url", "ipath")
        # TODO read from config and prepare db

        # TODO confdir = CONFIG_DIR
        confdir="/home/tender/.recoll"
        extra_dbs = []
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
            recollQueryModel.Role_FILE_STATUS: QByteArray(b'fileStatus')
        }
        return roles

    def rowCount(self, parent):
        ret = len(self.searchResults)
        print("RecollQuery.rowCount(): %d"% ret)
        return ret

    # def columnCount(self, parent):
    #     print("RecollQuery.columnCount()")
    # if parent.isValid():
    #     return 0
    # else:
    #     return len(self.attrs)

    def setquery(self, db, q, sortfield="", ascending=True):
        """Parse and execute query on open db"""
        # print("RecollQuery.setquery():")
        # Get query object
        if q=="":
            return
        self.query = db.query()
        if sortfield:
            self.query.sortby(sortfield, ascending)
        # Parse/run input query string
        self.totres = self.query.execute(q)
        print("total res:",self.totres)
        self.qtext = q
        self.db = db
        self.beginResetModel()
        self.searchResults = []
        self.endResetModel()
        if self.canFetchMore(None):
            self.fetchMore(None)


    def getdoc(self, index):
        if index.row() < len(self.searchResults):
            return self.searchResults[index.row()]
        else:
            return None

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

        if index.row() <0:
            return None
        if index.row() >= len(self.searchResults):
            return None

        d = self.searchResults[index.row()]
        if role == recollQueryModel.Role_FILE_NAME:
            # print("RecollQuery.data: row %d, col %d role: %s" % \
            #     (index.row(), index.column() role))
            return d['filename']
        elif role == recollQueryModel.Role_LOCATION:
            return d['url']
        elif role == recollQueryModel.Role_FILE_SIMPLE_CONTENT:
            return d['abstract']
        elif role == recollQueryModel.Role_FILE_STATUS:
            pass
        return

    def canFetchMore(self, parent):
        print("RecollQuery.canFetchMore:", self.searchResults," ",self.totres)
        if len(self.searchResults) < self.totres:
            return True
        else:
            return False

    def fetchMore(self, parent):
        print("RecollQuery.fetchMore:")
        num_to_fetch=min(self.pagelen,self.totres-len(self.searchResults))
        self.beginInsertRows(QtCore.QModelIndex(), len(self.searchResults),
                             len(self.searchResults) + num_to_fetch-1)
        for count in range(num_to_fetch):
            try:
                self.searchResults.append(self.query.fetchone())
                print("insert ",count)
            except:
                print("except",count)
                # break
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
        newdoc = textextract(self.currentdoc)
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
        print("on_savePB_clicked(self)")
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

# class recollQueryModel(QAbstractListModel):
#    def __init__(self):
#        QAbstractListModel.__init__(self)
#
#        self.__cpu_count = random.randint(0,10)
#        self.__cpu_load = [0] * self.__cpu_count
#
#        self.__update_timer = QTimer(self)
#        self.__update_timer.setInterval(1000)
#        self.__update_timer.timeout.connect(self.__update)
#        self.__update_timer.start()
#
#        # The first call returns invalid data
#
#    def __update(self):
#        self.__cpu_load = random_int_list(0,100,10)
#        self.dataChanged.emit(self.index(0, 0), self.index(self.__cpu_count - 1, 0))
#
#    def rowCount(self, parent):
#        return self.__cpu_count
#
#    def data(self, index, role):
#        if (role == Qt.DisplayRole and
#                index.row() >= 0 and
#                index.row() < len(self.__cpu_load) and
#                index.column() == 0):
#            return self.__cpu_load[index.row()]
#        else:
#            return None

#include "recollquerymodel.h"

RecollQueryModel::RecollQueryModel()
{

}

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
           print("query change to:\""+val+"\"")
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

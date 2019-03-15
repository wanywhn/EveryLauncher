from PySide2.QtGui import QIcon
from PySide2.QtWidgets import QSystemTrayIcon, QMenu, QAction
from PySide2.QtCore import Property, Signal,Slot


class SystemTray(QSystemTrayIcon):

    @classmethod
    def get_instance(cls, parent):
        return SystemTray(QIcon.fromTheme("system-help"), parent)

    def _readMainWindowVisiable(self):
        return self.mainWindowVisibility

    mainWindowVisiableChanged = Signal(bool)

    def _writeMainWindowVisiable(self, f):
        if self.mainWindowVisibility != f:
            self.mainWindowVisibility = f
            self.mainWindowVisiableChanged.emit(self.mainWindowVisibility)

    mainWindowVisiable = Property(bool,_readMainWindowVisiable,_writeMainWindowVisiable,
                                  notify=mainWindowVisiableChanged)

    def __init__(self, icon, parent):
        QSystemTrayIcon.__init__(self, icon, parent)
        self.icon = QIcon.fromTheme("system-help")
        self.init_ui()
        self.init_conn()
        self.mainWindowVisibility = True

    def init_ui(self):
        self.trayMenu = QMenu(self.parent())

        self.showUp = QAction(self.icon, self.tr("ShowMainWindow"), self.trayMenu)
        self.preferenceAction = QAction(self.icon, self.tr("Preference"), self.trayMenu)
        self.exitAction = QAction(self.icon, self.tr("Exit"), self.trayMenu)

        self.trayMenu.addAction(self.showUp)
        self.trayMenu.addAction(self.preferenceAction)
        self.trayMenu.addAction(self.exitAction)
        self.setContextMenu(self.trayMenu)

    def init_conn(self):
        self.showUp.triggered.connect(lambda :self.showMainWindow(self.mainWindowVisibility!=True))

    @Slot(bool)
    def showMainWindow(self, f):
        print("visibility change to ",f)
        self._writeMainWindowVisiable(f)

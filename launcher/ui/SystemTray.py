import subprocess

from PySide2.QtGui import QIcon
from PySide2.QtWidgets import QSystemTrayIcon, QMenu, QAction, QMessageBox
from PySide2.QtCore import Property, Signal, Slot

from launcher.config import RECOLL_CONFIG_DIR
from launcher.ui.PreferenceWindow import PreferenceWindow


class SystemTray(QSystemTrayIcon):

    @classmethod
    def get_instance(cls, parent):
        return SystemTray(QIcon.fromTheme("system-help"), parent)

    def _readMainWindowVisiable(self):
        return self._mainWindowVisibility

    mainWindowVisiableChanged = Signal(bool)

    def _writeMainWindowVisiable(self, f):
        if self._mainWindowVisibility != f:
            self._mainWindowVisibility = f
            self.mainWindowVisiableChanged.emit(self._mainWindowVisibility)

    mainWindowVisiable = Property(bool, _readMainWindowVisiable, _writeMainWindowVisiable,
                                  notify=mainWindowVisiableChanged)

    def __init__(self, icon, parent):
        QSystemTrayIcon.__init__(self, icon, parent)
        self._icon = QIcon.fromTheme("system-help")
        self._init_ui()
        self._init_conn()
        self._mainWindowVisibility = True

    def _init_ui(self):
        self.trayMenu = QMenu(self.parent())

        self.showUp = QAction(self._icon, self.tr("ShowMainWindow"), self.trayMenu)
        self.preferenceAction = QAction(self._icon, self.tr("Preference"), self.trayMenu)
        self.exitAction = QAction(self._icon, self.tr("Exit"), self.trayMenu)

        self.trayMenu.addAction(self.showUp)
        self.trayMenu.addAction(self.preferenceAction)
        self.trayMenu.addAction(self.exitAction)
        self.setContextMenu(self.trayMenu)

    def _init_conn(self):
        self.showUp.triggered.connect(lambda: self.showMainWindow(self._mainWindowVisibility != True))
        self.preferenceAction.triggered.connect(self.preferenceActionTriggered)

    # @Slot()
    def preferenceActionTriggered(self):
        pw = PreferenceWindow(self.parent())
        pw.show()

    def fileChanged(self,file_list:list):
        #TODO index new file
        if len(file_list)<=0:
            return 
        print("command arg is " " -c " ,RECOLL_CONFIG_DIR," -i " ,' '.join(file_list));

        subprocess.Popen(['recollindex',' -c '+RECOLL_CONFIG_DIR+ ' -i '+' '.join(file_list)]);

    @Slot(bool)
    def showMainWindow(self, f):
        if f is None:
            f = not self._readMainWindowVisiable()
        print("visibility change to ", f)
        self._writeMainWindowVisiable(f)
        if f:
            self.parent().show()
            self.parent().activateWindow()
        else:
            self.parent().hide()

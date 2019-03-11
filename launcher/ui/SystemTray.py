from PySide2.QtGui import QIcon
from PySide2.QtWidgets import QSystemTrayIcon, QMenu, QAction


class SystemTray(QSystemTrayIcon):

    @classmethod
    def get_instance(cls,parent):
        return SystemTray(QIcon.fromTheme("system-help"),parent)

    def __init__(self,icon,parent):
        QSystemTrayIcon.__init__(self,icon,parent)
        self.icon=QIcon.fromTheme("system-help")
        self.init_ui()
        self.init_conn()

    def init_ui(self):
        self.trayMenu=QMenu(self.parent())
        self.preferenceAction=QAction(self.icon,"Preference",self.trayMenu)
        self.exitAction=QAction(self.icon,"Exit",self.trayMenu)

        self.trayMenu.addAction(self.preferenceAction)
        self.trayMenu.addAction(self.exitAction)
        self.setContextMenu(self.trayMenu)
    def init_conn(self):
        pass


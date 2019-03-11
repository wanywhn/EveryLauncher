from PySide2 import QtWidgets,QtCore
from PySide2.QtWidgets import QVBoxLayout, QHBoxLayout, QPushButton, QLineEdit, QListView


class MainWindow(QtWidgets.QWidget):
    def __init__(self, parent=None):
        QtWidgets.QWidget.__init__(self, parent)

        self.init_ui()
        self.init_conn()

    def init_conn(self):
        # self.connect(self.systemTray.preferenceAction,QtCore.SIGNAL("triggered()"),self.showPreference)
        # self.connect(self.systemTray.exitAction,QtCore.SIGNAL("Triggered()"),self.exitClicked)
        pass
    def init_ui(self):
        self.setWindowFlags(QtCore.Qt.WindowCloseButtonHint \
                            |QtCore.Qt.FramelessWindowHint)
        self.hcatelayout=QHBoxLayout()

        self.cate_dict={"all":"所有","app":"程序"
                        ,"text":"text","video":"video"
                        ,"music":"music"}
        for k in self.cate_dict.keys():
            setattr(self,"btn_"+k,QPushButton(self.cate_dict[k]))
            self.hcatelayout.addWidget(getattr(self,"btn_"+k))


        self.searchLine=QLineEdit()


        self.resultListView=QListView()
        ## TODO listView's heigh change

        self.vmlayout=QVBoxLayout()
        self.vmlayout.addLayout(self.hcatelayout)
        self.vmlayout.addWidget(self.searchLine)
        self.vmlayout.addWidget(self.resultListView)

        self.setLayout(self.vmlayout)

        # self.systemTray=SystemTray.get_instance(self)
        # self.systemTray.show()


    # TODO single
    @classmethod
    def get_instance(cls):
        return MainWindow()

    def showTray(self,show):
        if show:
            self.systemTray.show()
            ## TODO ,show or not

    def showPreference(self):
        self.systemTray.showMessage("test","t4est")
        pass

    def exitClicked(self):
        pass
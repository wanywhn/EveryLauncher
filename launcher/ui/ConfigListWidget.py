import sys

import PySide2.QtCore
import PySide2.QtWidgets
from PySide2.QtGui import QGuiApplication
from PySide2.QtCore import QStringListModel, Slot,QModelIndex
from PySide2.QtWidgets import QWidget, QVBoxLayout, QGroupBox, QHBoxLayout, QPushButton, QListView, QApplication,QFileDialog
import configparser

from launcher.config import RECOLL_CONFIG_FILE


class ConfigListWidget(QGroupBox):

    _confParser=configparser.ConfigParser()
    _confParser.optionxform=str
    with open(RECOLL_CONFIG_FILE) as f:
        file_content = '[DEFAULT]\n' + f.read()
    _confParser.read_string(file_content)
    def __init__(self, parent=None, showGroupName: str = None, section="DEFAULT",
                 groupName: str = None):
        super().__init__(showGroupName, parent)
        self._groupName = groupName
        self._section = section
        self.init_ui()

        self._confParser = None
        # TODO exception
        self.init_data()
        self._init_conn()

    def _init_conn(self):
        self._btnAdd.clicked.connect(self._btn_add)
        self._btnDelete.clicked.connect(self._btn_delete)
        pass

    def _btn_add(self):
        path=QFileDialog.getExistingDirectory(self,caption="Select Dir",dir="~")
        slist=self._listmodel.stringList()
        slist.append(path)
        self._listmodel.setStringList(slist)
        pass
    def _btn_delete(self):
        select=self._listView.selectedIndexes()
        for item in select:
            self._listmodel.removeRow(item.row())


    # @classmethod
    def save_values(self):
        row=""
        #TODO remove duplicated
        for i in range(self._listmodel.rowCount()):
            row += "\t"+self._listmodel.index(i).data()+"\\\n"

        row=row[:-2]
        print(row)
        # self._confParser[self._section][self._groupName]=row
        ConfigListWidget._confParser.set(self._section,self._groupName,row)
        with open(RECOLL_CONFIG_FILE,'w+') as conf:
            ConfigListWidget._confParser.write(conf)
            conf.seek(0)
            lines=conf.readlines()

        with open(RECOLL_CONFIG_FILE,'w') as conf:
            for i in lines[1:]:
                conf.write(i)


    def init_data(self):
        # TODO default values
        self._listmodel = QStringListModel(
            str(ConfigListWidget._confParser[self._section][self._groupName]).replace("\\", "").strip().replace("\t","").splitlines())
        self._listView.setModel(self._listmodel)


    def init_ui(self):
        self._vlayout = QVBoxLayout()
        self.setLayout(self._vlayout)
        self._listView = QListView()
        self._hlayout = QHBoxLayout()

        self._vlayout.addWidget(self._listView)
        self._vlayout.addLayout(self._hlayout)

        self._btnAdd = QPushButton("Add")
        self._btnDelete = QPushButton("Delete")

        self._hlayout.addWidget(self._btnAdd)
        self._hlayout.addWidget(self._btnDelete)


if __name__ == "__main__":
    app = QApplication()
    widget = ConfigListWidget(showGroupName="Topdir", confName="/home/tender/.config/everylauncher/recoll/recoll.conf",
                              groupName="topdirs")
    widget.show()

    sys.exit(app.exec_())

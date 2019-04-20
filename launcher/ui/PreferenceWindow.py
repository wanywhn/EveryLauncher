from PySide2.QtCore import QSettings
from PySide2.QtWidgets import QDialog, QTabWidget, QDialogButtonBox, QVBoxLayout, QMessageBox, QWidget, QHBoxLayout, \
    QFormLayout, QCheckBox, QGroupBox, QLabel, QPushButton

from launcher.config import SHOW_WINDOW_ON_START, SHOW_INDICATOR, ORGANIZATION_NAME, APPLICATION_NAME
from launcher.ui.ConfigListWidget import ConfigListWidget
from launcher.ui.IndexSche import IndexSche
from launcher.utils.utils import trans_to_bool


#TODO check state in main.py
class PreferenceWindow(QDialog):


    def __init__(self, parent):
        super().__init__(parent)
        self._setting = QSettings()  # ORGANIZATION_NAME, APPLICATION_NAME)
        self._tabWidget = QTabWidget()
        self._tabGeneral = QWidget()
        self._tabIndex = QWidget()
        self._tabIndexSche=QWidget()

        self._ckb_show_window_on_start = QCheckBox()
        self._ckb_show_indicator = QCheckBox()

        self._buttonBox = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)

        self._init_ui()

        self._init_conn()
        self._read_settings()

    def _read_settings(self):
        self._setting.beginGroup("General")
        val= self._setting.value(SHOW_WINDOW_ON_START, True)
        self._ckb_show_window_on_start.setChecked(trans_to_bool(val))
        val= str(self._setting.value(SHOW_INDICATOR, True))
        self._ckb_show_indicator.setChecked(trans_to_bool(val))
        self._setting.endGroup()

    def _write_settings(self):
        self._setting.beginGroup("General")
        self._setting.setValue(SHOW_WINDOW_ON_START, self._ckb_show_window_on_start.isChecked())
        self._setting.setValue(SHOW_INDICATOR, self._ckb_show_indicator.isChecked())
        self._setting.endGroup()

    def _init_ui(self):
        vmlayout=QVBoxLayout()
        self._tabGeneral.setLayout(vmlayout)

        general_group_box=QGroupBox(self.tr("General"))
        vmlayout.addWidget(general_group_box)

        # general_hmlayout = QHBoxLayout()

        general_left_formlayout = QFormLayout()
        general_group_box.setLayout(general_left_formlayout)

        general_left_formlayout.addRow(self.tr("Show Window on Start"), self._ckb_show_window_on_start)
        general_left_formlayout.addRow(self.tr("Show Indicator"), self._ckb_show_indicator)

        general_group_index=QGroupBox(self.tr("Index"))
        vmlayout.addWidget(general_group_index)

        general_index_formlayout=QFormLayout()
        general_group_index.setLayout(general_index_formlayout)

        self._general_index_update=QPushButton(self.tr("Update Index"))
        self._general_index_reindex=QPushButton(self.tr("Reindex"))

        general_index_formlayout.addWidget(self._general_index_update)
        general_index_formlayout.addWidget(self._general_index_reindex)



        index_vlayout=QVBoxLayout()
        self._tabIndex.setLayout(index_vlayout)

        self._topDir=ConfigListWidget(showGroupName="TopDir",groupName="topdirs")
        self._skipDir=ConfigListWidget(showGroupName="SkipDir",groupName="skippedPaths")
        # self._cigen=ConfigListWidget("CiGen")
        index_vlayout.addWidget(self._topDir)
        index_vlayout.addWidget(self._skipDir)
        # self._index_vlayout.addWidget(self._cigen)


        self._tabIndexSche=IndexSche()

        # self._tabIndex.setLayout(index_vmlayout)

        self._tabWidget.addTab(self._tabGeneral, self.tr("General"))
        self._tabWidget.addTab(self._tabIndex, self.tr("Global Index"))
        self._tabWidget.addTab(self._tabIndexSche,self.tr("Sched"))

        main_layout = QVBoxLayout()
        main_layout.addWidget(self._tabWidget)
        main_layout.addWidget(self._buttonBox)
        self.setLayout(main_layout)

    def _init_conn(self):
        self._buttonBox.accepted.connect(self.accept_changes)
        self._buttonBox.rejected.connect(self.reject_changes)

    def accept_changes(self):
        self._write_settings()
        self._skipDir.save_values()
        self._topDir.save_values()
        self.close()

    def reject_changes(self):
        self.close()

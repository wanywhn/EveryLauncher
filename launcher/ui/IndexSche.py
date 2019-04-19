# import PySide2.QtCore
# import PySide2.QtWidgets
import sys

from PySide2.QtWidgets import QWidget, QVBoxLayout, QRadioButton, QGroupBox, QFormLayout, QLabel, QLineEdit, \
    QPushButton, QHBoxLayout, QApplication, QCheckBox, QDialogButtonBox


class IndexSche(QWidget):
    def __init__(self):
        super().__init__()
        self._init_ui()
        self._init_conn()

    def _init_ui(self):
        # self._btn_cron=QRadioButton(self.tr("Cron"))
        # self._btn_realtime=QRadioButton(self.tr("RealTime"))
        vlayout = QVBoxLayout()
        self.setLayout(vlayout)

        _group_cron = QGroupBox(self.tr("Cron"))
        vlayout.addWidget(_group_cron)

        _formLayout_Cron = QFormLayout()
        _group_cron.setLayout(_formLayout_Cron)

        _formLayout_Cron.addWidget(QLabel("xxxx"))
        self._label_cron_week = QLineEdit()
        self._label_cron_hour = QLineEdit()
        self._label_cron_minu = QLineEdit()
        _formLayout_Cron.addRow(self.tr("星期日"), self._label_cron_week)
        _formLayout_Cron.addRow(self.tr("小时"), self._label_cron_hour)
        _formLayout_Cron.addRow(self.tr("分钟"), self._label_cron_minu)

        self._btn_enable = QPushButton(self.tr("Enable"))
        self._btn_disable = QPushButton(self.tr("Disable"))
        # self._btn_cancel = QPushButton(self.tr("Cancel"))
        hlayout = QHBoxLayout()
        hlayout.addWidget(self._btn_enable)
        hlayout.addWidget(self._btn_disable)
        # hlayout.addWidget(self._btn_cancel)
        # self._btn_cron=QDialogButtonBox(QDialogButtonBox)

        _formLayout_Cron.addWidget(QLabel("ddd"))
        _formLayout_Cron.addRow(hlayout)

        _group_realtime = QGroupBox(self.tr("RealTime"))
        vlayout.addWidget(_group_realtime)

        formLayout=QFormLayout()
        _group_realtime.setLayout(formLayout)

        formLayout.addWidget(QLabel(self.tr("xxx")))

        self._cbk_startOnStart=QCheckBox()
        self._cbk_startNow=QCheckBox()
        self._cbk_startNow.setEnabled(False)
        formLayout.addRow(self._cbk_startOnStart,QLabel(self.tr("Start on System Start")))
        formLayout.addRow(self._cbk_startNow,QLabel(self.tr("Start Now")))



        pass

    def _init_conn(self):
        self._btn_enable.clicked.connect(self._cron_enable)
        self._btn_disable.clicked.connect(self._cron_disable)
        self._cbk_startOnStart.toggled.connect(self._cbk_onstart_toggled)
        self._cbk_startNow.toggled.connect(self._cbk_start_now_toggled)
        pass
    def _cbk_onstart_toggled(self,checked):
        if checked:
            self._cbk_startNow.setEnabled(True)
        ## TODO imple


        pass
    def _cbk_start_now_toggled(self,checked):
        pass
    def _cron_enable(self):
        pass
    def _cron_disable(self):
        pass


if __name__ == "__main__":
    app = QApplication()
    index = IndexSche()
    index.show()

    sys.exit(app.exec_())

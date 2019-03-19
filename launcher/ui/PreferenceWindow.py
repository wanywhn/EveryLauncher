from PySide2.QtCore import QSettings
from PySide2.QtWidgets import QDialog, QTabWidget, QDialogButtonBox, QVBoxLayout, QMessageBox


class PreferenceWindow(QDialog):
    def __init__(self,parent):
        super().__init__(parent)
        self._msetting=QSettings()
        self.init_ui()
        self.init_conn()

    def init_ui(self):
        self._tabWidget =QTabWidget()

        self._buttonBox=QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)

        mainLayout=QVBoxLayout()
        mainLayout.addWidget(self._tabWidget)
        mainLayout.addWidget(self._buttonBox)
        self.setLayout(mainLayout)

    def init_conn(self):
        self._buttonBox.accepted.connect(self.acceptChanges)
        self._buttonBox.rejected.connect(self.rejectChanges)

    def acceptChanges(self):
        QMessageBox.information(self,"accept","accept")
    def rejectChanges(self):
        QMessageBox.warning(self,"warning","reject")

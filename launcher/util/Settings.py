from PySide2.QtCore import QSettings


class MSettings(QSettings):
    def __init__(self):
        super().__init__()


    @classmethod
    def get_instance(cls):
        return MSettings()

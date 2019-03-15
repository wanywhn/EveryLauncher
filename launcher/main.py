from __future__ import print_function
import sys
import os
import signal
import logging
from threading import Event


import dbus
import dbus.service
from PySide2.QtCore import QUrl, QSortFilterProxyModel
from PySide2.QtQml import QQmlApplicationEngine
from PySide2.QtWidgets import QApplication
from dbus.mainloop.glib import DBusGMainLoop

from launcher.ui.SystemTray import SystemTray
from launcher.config import (get_version, get_options, is_wayland, is_wayland_compatibility_on,
                     gdk_backend, CACHE_DIR, CONFIG_DIR)

from launcher.ui.model.recollQueryModel import recollQueryModel

DBUS_SERVICE = 'com.gitee.wanywhn.everylauncher'
DBUS_PATH = '/com/gitee/wanywhn/evertlauncher'


# TODO create index config file
def _create_dirs():
    if not os.path.exists(CONFIG_DIR):
        os.makedirs(CONFIG_DIR)

    # make sure ~/.cache/ulauncher exists
    if not os.path.exists(CACHE_DIR):
        os.makedirs(CACHE_DIR)


class EveryLauncherDbusService(dbus.service.Object):
    def __init__(self, window):
        self.tray = window
        bus_name = dbus.service.BusName(DBUS_SERVICE, bus=dbus.SessionBus())
        super(EveryLauncherDbusService, self).__init__(bus_name, DBUS_PATH)

    @dbus.service.method(DBUS_SERVICE)
    def toggle_window(self):
        self.tray.showMainWindow(True)


class SignalHandler(object):

    _exit_event = None
    _app_window = None
    _logger = None

    def __init__(self, app_window):
        self._exit_event = Event()
        self._app_window = app_window
        self._logger = logging.getLogger('everylauncher')
        signal.signal(signal.SIGINT, self._exit_gracefully)
        signal.signal(signal.SIGTERM, self._exit_gracefully)
        signal.signal(signal.SIGHUP, self._reload_configs)

    def _reload_configs(self, *args):
        self._logger.info('Received SIGHUP. Reloading configs')
        self._app_window.init_theme()

    def killed(self):
        """
        :rtype: bool
        """
        return self._exit_event.is_set()

    def _exit_gracefully(self, signum, frame):
        self._exit_event.set()


def main():
    """
    Main function that starts everything
    """
    if is_wayland() and gdk_backend().lower() != 'x11' and not is_wayland_compatibility_on():
        warn = """
                    [!]
        Looks like you are in Wayland session
        Please run Ulauncher with env var
        GDK_BACKEND set to 'x11' like this:
        GDK_BACKEND=x11 ulauncher
        """
        print(warn, file=sys.stderr)
        sys.exit(1)

    # start DBus loop
    DBusGMainLoop(set_as_default=True)
    bus = dbus.SessionBus()
    instance = bus.request_name(DBUS_SERVICE)

    if instance != dbus.bus.REQUEST_NAME_REPLY_PRIMARY_OWNER:
        toggle_window = dbus.SessionBus().get_object(DBUS_SERVICE, DBUS_PATH).get_dbus_method("toggle_window")
        toggle_window()
        # return

    _create_dirs()

    options = get_options()
    # setup_logging(options)
    logger = logging.getLogger('everylauncher')
    logger.info('EveryLauncher version %s' % get_version())
    logger.info("Is Wayland: %s" % is_wayland())
    logger.info("Wayland compatibility: %s" % ('on' if is_wayland_compatibility_on() else 'off'))

    # log uncaught exceptions
    # def except_hook(exctype, value, tb):
    #     logger.error("Uncaught exception", exc_info=(exctype, value, tb))

    # sys.excepthook = except_hook

    app=QApplication(sys.argv)
    engine = QQmlApplicationEngine()

    model=recollQueryModel()
    proxy=QSortFilterProxyModel()
    proxy.setSourceModel(model)
    proxy.setFilterRole(recollQueryModel.Role_TYPE)

    # qmlRegisterType(recollQueryModel, 'RecollQuery', 1, 0, 'EveryQueryModel')

    tray=SystemTray.get_instance(None)

    engine.rootContext().setContextProperty("queryModel",model)
    engine.rootContext().setContextProperty("filterModel",proxy)
    engine.rootContext().setContextProperty("systemTray",tray)


    EveryLauncherDbusService(tray)
    # if not options.hide_window:
    #     tray.showMainWindow(False)
    tray.show()
    engine.load(QUrl("ui/QML/main.qml"))
    if not engine.rootObjects():
        sys.exit(-1)

    # if Settings.get_instance().get_property('show-indicator-icon'):


    # workaround to make Ctrl+C quiting the app
    # signal_handler = SignalHandler(window)

    sys.exit(app.exec_())


main()
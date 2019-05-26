cmake_minimum_required(VERSION 3.12)
project(EveryLauncher VERSION 1.0.0)

option(USE_DXCB "integration with dxcb patfprm plugin" OFF)
option(E_DEBUG "turn on debug output" OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(E_DEBUG ON)
endif ()

find_package(PkgConfig REQUIRED)
set(CMAKE_CXX_STANDARD 14)

INSTALL(FILES dbus/com.gitee.wanywhn.EveryLauncher.service
        DESTINATION /usr/share/dbus-1/services
        )
INSTALL(PROGRAMS bin/everylauncher-toggle
        DESTINATION bin
        )
INSTALL(DIRECTORY lib/
        DESTINATION lib
        )
INSTALL(FILES EveryLauncher.desktop
        DESTINATION share/applications
        )

INSTALL(FILES everylauncher.svg
        DESTINATION share/icons/hicolor/scalable/apps
        )
## !!!FIXME assume /usr
INSTALL(DIRECTORY recoll_conf
        DESTINATION share/everylauncher
        )
INSTALL(DIRECTORY filters
        DESTINATION share/everylauncher
        )
INSTALL(DIRECTORY icon
        DESTINATION share/everylauncher
        )


add_subdirectory(src)
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "
libqt5xdg-dev,libqt5x11extras5v,
libdtkwidget-dev,x11proto-record-dev, libxtst-dev,
libqt5webengine5,recollcmd,
libqt5webenginewidgets
")

#SET(CMAKE_DEBIAN_PACKAGE_ARCHITECTUREE "i386")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "一个全文搜索加桌面启动器。")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jia Qingtong (wanywhn@qq.com)")
INCLUDE(CPack)

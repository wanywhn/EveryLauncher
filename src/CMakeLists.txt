set(CMD_NAME everylauncher)


set(QTPKG Core Widgets Gui DBus WebEngineWidgets X11Extras Network Xml Multimedia WebEngine
        MultimediaWidgets
        )
list(TRANSFORM QTPKG PREPEND "Qt5")
foreach (item ${QTPKG})
    find_package(${item})
endforeach ()
#find_package(Qt5 REQUIRED COMPONENTS ${QTPKG})

find_package(Qt5Xdg)
pkg_check_modules(Dtk REQUIRED IMPORTED_TARGET dtkwidget)
pkg_check_modules(Dtk REQUIRED IMPORTED_TARGET dtkcore)

set(dbus_interface_xml com.gitee.wanywhn.EveryLauncher.xml)

## FIXME why not work
qt5_generate_dbus_interface(
        ${CMAKE_CURRENT_SOURCE_DIR}}/dbusproxy.h
        ${dbus_interface_xml}
        OPTIONS -a
)
qt5_add_dbus_interface(
        QTRES
        ${dbus_interface_xml}
        everylauncher_interface
#        EveryLauncherInterface
        )

qt5_add_resources(QTRES res.qrc)
qt5_add_dbus_adaptor(
        QTRES
        ${dbus_interface_xml}
        dbusproxy.h
        DBusProxy
        everylauncher_adaptor
        EveryLauncherAdaptor
)

aux_source_directory(. SRC)
aux_source_directory(./Detailed SRC)
aux_source_directory(./Detailed/player SRC)
aux_source_directory(./confgui SRC)
aux_source_directory(./Model SRC)

qt5_wrap_ui(FORMS widget.ui)
add_executable(${CMD_NAME} ${SRC} ${QTRES} ${FORMS} Detailed/map_preview.cpp Detailed/map_preview.h Model/map_model.cpp Model/map_model.h Model/ELModelInterface.cpp confgui/ConfigModelEngin.cpp confgui/ConfigModelEngin.h)



#list(TRANSFORM QTPKG PREPEND "Qt5::")
list(TRANSFORM QTPKG REPLACE "Qt5" "Qt5::")
foreach (item ${QTPKG})
    target_link_libraries(${CMD_NAME} PUBLIC ${item})
endforeach ()

target_link_directories(${CMD_NAME} PRIVATE ../3rdparty/qtpdf/lib  PRIVATE /usr/lib/recoll)

set(LLIBS X11 Xext Xtst Qt5Pdf recoll Qt5PdfWidgets )

target_link_libraries(${CMD_NAME} PUBLIC ${LLIBS})
target_link_libraries(${CMD_NAME} PUBLIC Qt5Xdg PkgConfig::Dtk)

set(THIRD_INCLUDEPATH ../3rdparty/recoll1-code/src/query
        ../3rdparty/recoll1-code/src/utils
        ../3rdparty/recoll1-code/src/rcldb
        ../3rdparty/recoll1-code/src/internfile
        ../3rdparty/recoll1-code/src/unac
        ../3rdparty/recoll1-code/src/common
        ../3rdparty/recoll1-code/src/qtgui
        ../3rdparty/qtpdf/include
        ../3rdparty/qtpdf/include/QtPdf

        ${CMAKE_BINARY_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}

        )
target_include_directories(${CMD_NAME} PRIVATE ${THIRD_INCLUDEPATH})

set_target_properties(${CMD_NAME} PROPERTIES AUTOMOC TRUE)

# Builds the QMLTermWidget QML plugin from the qmltermwidget submodule
# sources, keeping the same on-disk layout as its qmake project:
#
#   QMLTermWidget/
#     libqmltermwidget.so
#     qmldir
#     QMLTermScrollbar.qml
#     color-schemes/
#     kb-layouts/

set(QTW_DIR "${PROJECT_SOURCE_DIR}/qmltermwidget")
set(QTW_LIB "${QTW_DIR}/lib")
set(QTW_SRC "${QTW_DIR}/src")
set(QTW_OUTPUT_DIR "${CMAKE_BINARY_DIR}/qmltermwidget/QMLTermWidget")

add_library(qmltermwidget MODULE
    ${QTW_LIB}/BlockArray.cpp
    ${QTW_LIB}/ColorScheme.cpp
    ${QTW_LIB}/Emulation.cpp
    ${QTW_LIB}/Filter.cpp
    ${QTW_LIB}/History.cpp
    ${QTW_LIB}/HistorySearch.cpp
    ${QTW_LIB}/KeyboardTranslator.cpp
    ${QTW_LIB}/konsole_wcwidth.cpp
    ${QTW_LIB}/kprocess.cpp
    ${QTW_LIB}/kpty.cpp
    ${QTW_LIB}/kptydevice.cpp
    ${QTW_LIB}/kptyprocess.cpp
    ${QTW_LIB}/ksession.cpp
    ${QTW_LIB}/ProcessInfo.cpp
    ${QTW_LIB}/Pty.cpp
    ${QTW_LIB}/Screen.cpp
    ${QTW_LIB}/ScreenWindow.cpp
    ${QTW_LIB}/Session.cpp
    ${QTW_LIB}/ShellCommand.cpp
    ${QTW_LIB}/TerminalCharacterDecoder.cpp
    ${QTW_LIB}/TerminalDisplay.cpp
    ${QTW_LIB}/tools.cpp
    ${QTW_LIB}/Vt102Emulation.cpp

    # Headers (from lib.pri) so AUTOMOC sees every Q_OBJECT class.
    ${QTW_LIB}/BlockArray.h
    ${QTW_LIB}/CharacterColor.h
    ${QTW_LIB}/Character.h
    ${QTW_LIB}/ColorScheme.h
    ${QTW_LIB}/ColorTables.h
    ${QTW_LIB}/DefaultTranslatorText.h
    ${QTW_LIB}/Emulation.h
    ${QTW_LIB}/ExtendedDefaultTranslator.h
    ${QTW_LIB}/Filter.h
    ${QTW_LIB}/History.h
    ${QTW_LIB}/HistorySearch.h
    ${QTW_LIB}/KeyboardTranslator.h
    ${QTW_LIB}/konsole_wcwidth.h
    ${QTW_LIB}/kprocess.h
    ${QTW_LIB}/kptydevice.h
    ${QTW_LIB}/kpty.h
    ${QTW_LIB}/kpty_p.h
    ${QTW_LIB}/kptyprocess.h
    ${QTW_LIB}/LineFont.h
    ${QTW_LIB}/Pty.h
    ${QTW_LIB}/ProcessInfo.h
    ${QTW_LIB}/Screen.h
    ${QTW_LIB}/ScreenWindow.h
    ${QTW_LIB}/Session.h
    ${QTW_LIB}/ShellCommand.h
    ${QTW_LIB}/TerminalCharacterDecoder.h
    ${QTW_LIB}/TerminalDisplay.h
    ${QTW_LIB}/tools.h
    ${QTW_LIB}/Vt102Emulation.h
    ${QTW_LIB}/mac-vkcode.h
    ${QTW_LIB}/ksession.h

    ${QTW_SRC}/qmltermwidget_plugin.cpp
    ${QTW_SRC}/qmltermwidget_plugin.h
)

target_include_directories(qmltermwidget PRIVATE ${QTW_LIB} ${QTW_SRC})

target_compile_definitions(qmltermwidget PRIVATE
    QTERMWIDGET_LIBRARY
    HAVE_POSIX_OPENPT
    HAVE_SYS_TIME_H
    $<$<PLATFORM_ID:Darwin>:HAVE_UTMPX _UTMPX_COMPAT HAVE_PTSNAME HAVE_UNLOCKPT HAVE_GRANTPT>
)

target_link_libraries(qmltermwidget PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::Widgets
)

set_target_properties(qmltermwidget PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${QTW_OUTPUT_DIR}"
)
if(APPLE)
    set_target_properties(qmltermwidget PROPERTIES SUFFIX ".dylib")
endif()

# Runtime assets next to the plugin.
add_custom_command(TARGET qmltermwidget POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${QTW_SRC}/qmldir" "${QTW_SRC}/QMLTermScrollbar.qml" "${QTW_OUTPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${QTW_LIB}/color-schemes" "${QTW_OUTPUT_DIR}/color-schemes"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${QTW_LIB}/kb-layouts" "${QTW_OUTPUT_DIR}/kb-layouts"
    VERBATIM
)

if(NOT APPLE)
    set(QTW_INSTALL_DIR "${QT6_INSTALL_QML}/QMLTermWidget")
    install(TARGETS qmltermwidget LIBRARY DESTINATION "${QTW_INSTALL_DIR}")
    install(FILES "${QTW_SRC}/qmldir" "${QTW_SRC}/QMLTermScrollbar.qml"
        DESTINATION "${QTW_INSTALL_DIR}")
    install(DIRECTORY "${QTW_LIB}/color-schemes" "${QTW_LIB}/kb-layouts"
        DESTINATION "${QTW_INSTALL_DIR}")
endif()

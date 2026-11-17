cmake_minimum_required(VERSION 3.21)

project(cool-retro-term LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Gui
    Network
    Qml
    Quick
    QuickControls2
    ShaderTools
    Sql
    Widgets
)

qt_standard_project_setup(REQUIRES 6.5)
set(CMAKE_AUTORCC ON)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/qmltermwidget/lib/Session.cpp"
   OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/KDSingleApplication/src/kdsingleapplication.cpp")
    message(FATAL_ERROR
        "Git submodules are missing. Run: git submodule update --init --recursive")
endif()

# The application binary goes to the top of the build tree and the
# QMLTermWidget plugin to <build>/qmltermwidget/QMLTermWidget, which is
# one of the import paths main.cpp adds relative to the executable.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_subdirectory(cmake/qmltermwidget)
add_subdirectory(app)

if(UNIX AND NOT APPLE)
    install(FILES cool-retro-term.desktop
        DESTINATION "${CMAKE_INSTALL_DATADIR}/applications")
    install(FILES packaging/cool-retro-term.metainfo.xml
        DESTINATION "${CMAKE_INSTALL_DATADIR}/metainfo")
    install(FILES packaging/cool-retro-term.1
        DESTINATION "${CMAKE_INSTALL_MANDIR}/man1")
endif()

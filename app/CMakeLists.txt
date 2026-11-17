set(APP_VERSION "unknown")
find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" -C "${PROJECT_SOURCE_DIR}" describe --tags --always --dirty=-dirty
        OUTPUT_VARIABLE GIT_DESCRIBE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(GIT_DESCRIBE)
        set(APP_VERSION "${GIT_DESCRIBE}")
    endif()
endif()
message(STATUS "cool-retro-term version: ${APP_VERSION}")

set(KDSA_DIR "${PROJECT_SOURCE_DIR}/KDSingleApplication/src")

qt_add_executable(cool-retro-term MACOSX_BUNDLE
    main.cpp
    fileio.cpp fileio.h
    fontmanager.cpp fontmanager.h
    fontlistmodel.cpp fontlistmodel.h

    ${KDSA_DIR}/kdsingleapplication.cpp
    ${KDSA_DIR}/kdsingleapplication.h
    ${KDSA_DIR}/kdsingleapplication_lib.h
    ${KDSA_DIR}/kdsingleapplication_localsocket.cpp
    ${KDSA_DIR}/kdsingleapplication_localsocket_p.h
)

target_include_directories(cool-retro-term PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${KDSA_DIR}"
)

target_compile_definitions(cool-retro-term PRIVATE
    APP_VERSION="${APP_VERSION}"
    KDSINGLEAPPLICATION_STATIC_BUILD
)

target_link_libraries(cool-retro-term PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Network
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Sql
    Qt6::Widgets
)

if(APPLE)
    target_link_libraries(cool-retro-term PRIVATE "-framework CoreFoundation")
    set(APP_ICNS "${CMAKE_CURRENT_SOURCE_DIR}/icons/crt.icns")
    target_sources(cool-retro-term PRIVATE "${APP_ICNS}")
    set_source_files_properties("${APP_ICNS}" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set_target_properties(cool-retro-term PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME "cool-retro-term"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.cool-retro-term.cool-retro-term"
        MACOSX_BUNDLE_ICON_FILE crt.icns
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${APP_VERSION}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${APP_VERSION}"
    )
endif()

add_dependencies(cool-retro-term qmltermwidget)

# QML /////////////////////////////////////////////////////////////////////////
# The interface is the CoolRetroTerm QML module, served from
# qrc:/qt/qml/CoolRetroTerm/. Files keep their paths relative to qml/.

set(QML_FILES
    Main.qml
    AboutDialog.qml
    ApplicationSettings.qml
    BurnInEffect.qml
    CheckableSlider.qml
    ColorButton.qml
    InsertNameDialog.qml
    PreprocessedTerminal.qml
    SettingsAdvancedTab.qml
    SettingsEffectsTab.qml
    SettingsGeneralTab.qml
    SettingsTerminalTab.qml
    SettingsWindow.qml
    ShaderTerminal.qml
    SimpleSlider.qml
    SizeOverlay.qml
    Storage.qml
    TerminalContainer.qml
    TerminalFrame.qml
    TerminalTabs.qml
    TerminalWindow.qml
    TimeManager.qml
    utils.js
    Components/SizedLabel.qml
    menus/FullContextMenu.qml
    menus/ShortContextMenu.qml
    menus/WindowMenu.qml
)
set(QML_RESOURCES
    images/allNoise512.png
    images/crt256.png
)

list(TRANSFORM QML_FILES PREPEND "qml/" OUTPUT_VARIABLE QML_FILE_PATHS)
list(TRANSFORM QML_RESOURCES PREPEND "qml/" OUTPUT_VARIABLE QML_RESOURCE_PATHS)
foreach(file IN LISTS QML_FILES QML_RESOURCES)
    set_source_files_properties("qml/${file}" PROPERTIES QT_RESOURCE_ALIAS "${file}")
endforeach()

# Generate qmldir files for Components/ and menus/ too (Qt 6.8+).
if(QT_KNOWN_POLICY_QTP0004)
    qt_policy(SET QTP0004 NEW)
endif()

qt_add_qml_module(cool-retro-term
    URI CoolRetroTerm
    VERSION 1.0
    QML_FILES ${QML_FILE_PATHS}
    RESOURCES ${QML_RESOURCE_PATHS}
    SOURCES
        fontmanager.cpp fontmanager.h
        fontlistmodel.cpp fontlistmodel.h
)

# Bundled fonts (FontManager loads them from :/fonts/) and the window icon.
set(FONT_FILES
    fonts/apple2/PrintChar21.ttf
    fonts/atari-400-800/AtariClassic-Regular.ttf
    fonts/bigblue-terminal/BigBlueTerm437NerdFontMono-Regular.ttf
    fonts/cozette/CozetteVector.ttf
    fonts/departure-mono/DepartureMonoNerdFontMono-Regular.otf
    fonts/fira-code/FiraCodeNerdFontMono-Regular.ttf
    fonts/fixedsys-excelsior/FSEX301-L2.ttf
    fonts/gohu/GohuFont11NerdFontMono-Regular.ttf
    fonts/greybeard/Greybeard-16px.ttf
    fonts/hack/HackNerdFontMono-Regular.ttf
    fonts/ibm-3278/3270NerdFontMono-Regular.ttf
    fonts/iosevka/IosevkaTermNerdFontMono-Regular.ttf
    fonts/jetbrains-mono/JetBrainsMonoNerdFontMono-Regular.ttf
    fonts/oldschool-pc-fonts/PxPlus_IBM_EGA_8x8.ttf
    fonts/oldschool-pc-fonts/PxPlus_IBM_VGA8.ttf
    fonts/oldschool-pc-fonts/PxPlus_IBM_VGA_8x16.ttf
    fonts/opendyslexic/OpenDyslexicMNerdFontMono-Regular.otf
    fonts/pet-me/PetMe.ttf
    fonts/pet-me/PetMe64.ttf
    fonts/source-code-pro/SauceCodeProNerdFontMono-Regular.ttf
    fonts/terminus/TerminessNerdFontMono-Regular.ttf
    fonts/unscii/unscii-16-full.ttf
    fonts/unscii/unscii-8-thin.ttf
    fonts/unscii/unscii-8.ttf
)
list(TRANSFORM FONT_FILES PREPEND "qml/" OUTPUT_VARIABLE FONT_FILE_PATHS)
qt_add_resources(cool-retro-term "fonts"
    PREFIX "/"
    BASE qml
    FILES ${FONT_FILE_PATHS}
)
set_source_files_properties(icons/32x32/cool-retro-term.png
    PROPERTIES QT_RESOURCE_ALIAS icons/cool-retro-term.png)
qt_add_resources(cool-retro-term "icon"
    PREFIX "/"
    FILES icons/32x32/cool-retro-term.png
)

# SHADERS /////////////////////////////////////////////////////////////////////
# Compiled with qsb at build time and embedded under qrc:/shaders/.

qt_add_shaders(cool-retro-term "shaders"
    PREFIX "/"
    FILES
        shaders/burn_in.frag
        shaders/burn_in.vert
        shaders/terminal_dynamic.vert
        shaders/terminal_frame.frag
        shaders/terminal_frame.vert
        shaders/terminal_static.vert
)

# The terminal fragment shaders are specialised at compile time: one variant
# per combination of enabled effects, picked at runtime by ShaderTerminal.qml.
foreach(raster_mode 0 1 2 3 4)
    foreach(burn_in 0 1)
        foreach(display_frame 0 1)
            foreach(chroma 0 1)
                set(variant "terminal_dynamic_raster${raster_mode}_burn${burn_in}_frame${display_frame}_chroma${chroma}")
                qt_add_shaders(cool-retro-term "shader_${variant}"
                    PREFIX "/"
                    DEFINES
                        CRT_RASTER_MODE=${raster_mode}
                        CRT_BURN_IN=${burn_in}
                        CRT_DISPLAY_FRAME=${display_frame}
                        CRT_CHROMA=${chroma}
                    FILES shaders/terminal_dynamic.frag
                    OUTPUTS shaders/${variant}.frag.qsb
                )
            endforeach()
        endforeach()
    endforeach()
endforeach()

foreach(rgb_shift 0 1)
    foreach(bloom 0 1)
        foreach(curvature 0 1)
            foreach(shine 0 1)
                set(variant "terminal_static_rgb${rgb_shift}_bloom${bloom}_curve${curvature}_shine${shine}")
                qt_add_shaders(cool-retro-term "shader_${variant}"
                    PREFIX "/"
                    DEFINES
                        CRT_RGB_SHIFT=${rgb_shift}
                        CRT_BLOOM=${bloom}
                        CRT_CURVATURE=${curvature}
                        CRT_FRAME_SHININESS=${shine}
                    FILES shaders/terminal_static.frag
                    OUTPUTS shaders/${variant}.frag.qsb
                )
            endforeach()
        endforeach()
    endforeach()
endforeach()

# INSTALL /////////////////////////////////////////////////////////////////////

install(TARGETS cool-retro-term
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    BUNDLE DESTINATION .
)

if(UNIX AND NOT APPLE)
    foreach(size 32 64 128 256)
        install(FILES "icons/${size}x${size}/cool-retro-term.png"
            DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/${size}x${size}/apps")
    endforeach()
endif()

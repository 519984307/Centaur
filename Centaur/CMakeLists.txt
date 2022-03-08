cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/Volumes/RicardoESSD/Qt/6.2.1/macos")
set(QWT_PATH ${QWT_PATH} /usr/local/Cellar/qwt/6.2.0_1/lib/qwt.framework/Versions/6)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Xml
        REQUIRED)

find_package(FMT CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_library(QWT Qwt /Volumes/RicardoESSD/dev/qwt/release/lib)


IF (WIN32)
    set(CENTAUR_EXECUTABLE WIN32)
ELSEIF (UNIX AND APPLE)
    set(CENTAUR_EXECUTABLE MACOSX_BUNDLE)
ELSE ()
    set(CENTAUR_EXECUTABLE)
ENDIF ()

add_executable(Centaur ${CENTAUR_EXECUTABLE}
        rcc/resource.qrc
        src/main.cpp
        src/CentaurApp.cpp
        src/CenListCtrl.cpp
        src/Logger.cpp
        src/HTMLDelegate.cpp
        src/Globals.cpp
        src/LoggerFormatter.cpp
        src/Plugins.cpp
        src/PluginsDialog.cpp
        include/CentaurApp.hpp
        include/CentaurGlobal.hpp
        include/CentaurUIState.hpp
        include/CenListCtrl.hpp
        include/Logger.hpp
        include/HTMLDelegate.hpp
        include/Globals.hpp
        include/PluginsDialog.hpp
        ../Plugins/include/CentaurPlugin.hpp
        ../Plugins/include/CentaurInterface.hpp
        ui/CentaurApp.ui
        ui/PluginsDialog.ui)

set_target_properties(Centaur PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER centaur.project.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})

target_include_directories(Centaur PRIVATE
        include
        ../../Plugins/include
        ../../Library/Protocol/include
        ../../Library/uuid/include
        ${QWT}/Headers)

target_link_libraries(Centaur PRIVATE fmt::fmt)
target_link_libraries(Centaur PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Xml)
target_link_libraries(Centaur PRIVATE ${QWT})
target_link_libraries(Centaur PRIVATE unofficial::sqlite3::sqlite3)

# Local Libraries
target_link_libraries(Centaur PRIVATE ${CMAKE_BINARY_DIR}/lib/libProtocol.dylib)
target_link_libraries(Centaur PRIVATE ${CMAKE_BINARY_DIR}/lib/libuuid.dylib)


target_compile_options(Centaur PRIVATE
        -Weverything
        -Wno-extra-semi-stmt
        -Wno-reserved-id-macro
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-c++20-compat
        -Wno-padded
        -fdiagnostics-color=always
        -Wno-poison-system-directories
        -Wno-redundant-parens
        -Wno-reserved-identifier)

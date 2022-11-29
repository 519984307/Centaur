cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_COLOR_DIAGNOSTICS ON)

find_package(FMT CONFIG REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(XercesC REQUIRED)
find_package(cryptopp CONFIG REQUIRED)

if (CMAKE_BUILD_TYPE EQUAL "Debug")
    find_library(QRENCODE_LIBRARY_DEBUG qrencoded)
    set(QRENCODE_LIBRARIES debug ${QRENCODE_LIBRARY_DEBUG})
else ()
    find_library(QRENCODE_LIBRARY_RELEASE qrencode)
    set(QRENCODE_LIBRARIES ${QRENCODE_LIBRARY_RELEASE})
endif ()


set(CMAKE_PREFIX_PATH /Volumes/RicardoESSD/Qt/6.3.0/macos)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Charts
        Sql
        WebSockets
        REQUIRED)


SET(CENT_SOURCE
        src/main.cpp
        src/CentaurApp.cpp
        src/Logger.cpp
        src/Globals.cpp
        src/Plugins.cpp
        src/PluginsDialog.cpp
        src/FavoritesDialog.cpp
        src/ConfigurationInterface.cpp
        src/ProtocolClient.cpp
        src/CandleViewWidget.cpp
        src/ProtocolServer.cpp
        src/LogDialog.cpp
        src/SplashDialog.cpp
        src/LoginDialog.cpp
        src/AddUserDialog.cpp
        src/SettingsDialog.cpp
        src/QRCodeDialog.cpp
        src/TOTP.cpp
        src/OrderbookDialog.cpp
        src/DepthChartDialog.cpp
        src/WatchlistWidget.cpp
        src/WatchlistModel.cpp
        src/PortfolioWidget.cpp
        src/PortfolioModel.cpp
        src/CandleChartDialog.cpp)

SET(CENT_INCLUDE
        include/CentaurApp.hpp
        include/CentaurGlobal.hpp
        include/CentaurUIState.hpp
        include/Logger.hpp
        include/Globals.hpp
        include/PluginsDialog.hpp
        include/FavoritesDialog.hpp
        include/ConfigurationInterface.hpp
        include/crc64.hpp
        include/ProtocolServer.hpp
        include/CandleViewWidget.hpp
        include/ProtocolClient.hpp
        include/CandleViewWidget.hpp
        ../Plugins/include/CentaurPlugin.hpp
        ../Plugins/include/CentaurInterface.hpp
        include/LogDialog.hpp
        include/SplashDialog.hpp
        include/AddUserDialog.hpp
        include/SettingsDialog.hpp
        include/LoginDialog.hpp
        include/QRCodeDialog.hpp
        include/TOTP.hpp
        include/OrderbookDialog.hpp
        include/DepthChartDialog.hpp
        include/WatchlistWidget.hpp
        include/WatchListModel.hpp
        include/PortfolioModel.hpp
        include/PortfolioWidget.hpp include/CandleChartDialog.hpp)

SET(CENT_UI
        ui/CentaurApp.ui
        ui/PluginsDialog.ui
        ui/FavoritesDialog.ui
        ui/CandleViewWidget.ui
        ui/LogDialog.ui
        ui/LoginDialog.ui
        ui/SettingsDialog.ui
        ui/AddUserDialog.ui
        ui/SplashDialog.ui
        ui/QRCodeDialog.ui)

SET(CENT_RESOURCE
        qrc/resource.qrc)

IF (UNIX AND NOT APPLE)
    message("Centaur Linux Detected")
ELSEIF (WIN32)
    message("Centaur Windows Detected")
    set(CENTAUR_EXECUTABLE WIN32)
ELSEIF (UNIX AND APPLE)
    message("Centaur Apple Detected")
    set(CENTAUR_EXECUTABLE MACOSX_BUNDLE)
    set(APPLE_PLATFORM TRUE)
    set(CMAKE_MACOSX_RPATH 1)
    set(CMAKE_INSTALL_RPATH "${CMAKE_BINARY_DIR}/lib")
ELSE ()
    message(FATAL_ERROR "Unknown target platform")
ENDIF ()

add_executable(Centaur ${CENTAUR_EXECUTABLE}
        ${CENT_RESOURCE}
        ${CENT_UI}
        ${CENT_INCLUDE}
        ${CENT_SOURCE}
        )


IF (${APPLE_PLATFORM})
    set(CENTAUR_EXECUTABLE MACOSX_BUNDLE)
    set(CMAKE_MACOSX_RPATH 1)
    set(CMAKE_INSTALL_RPATH "${CMAKE_BINARY_DIR}/lib")
ELSE ()
    message(FATAL_ERROR "Unknown target platform")
ENDIF ()


# Create the Apple Bundle
IF (${APPLE_PLATFORM})
    set_target_properties(Centaur PROPERTIES
            MACOSX_BUNDLE_BUNDLE_NAME Centaur
            MACOSX_BUNDLE_GUI_IDENTIFIER com.centaurproject.Centaur
            MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
            MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
ENDIF ()

target_include_directories(Centaur PRIVATE
        include
        ../Library/Protocol/include
        ../Plugins/include
        ../Library/uuid/include
        ../Library/cui/include
        ../include)

target_link_libraries(Centaur PRIVATE fmt::fmt)
target_link_libraries(Centaur PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Charts Qt6::Sql Qt6::WebSockets)
target_link_libraries(Centaur PRIVATE SQLite3)
target_link_libraries(Centaur PRIVATE XercesC::XercesC)
target_link_libraries(Centaur PRIVATE rapidjson)
target_link_libraries(Centaur PRIVATE ${QRENCODE_LIBRARIES})
target_link_libraries(Centaur PRIVATE cryptopp-static)

# Local Libraries
target_link_libraries(Centaur PRIVATE ${CMAKE_BINARY_DIR}/lib/libProtocol.dylib)
target_link_libraries(Centaur PRIVATE ${CMAKE_BINARY_DIR}/lib/libuuid.dylib)
target_link_libraries(Centaur PRIVATE ${CMAKE_BINARY_DIR}/lib/libcui.dylib)

if (${APPLE_PLATFORM})
    set(BUNDLE_BUILTIN_PLUGINS "${CMAKE_BINARY_DIR}/bin/Centaur.app/Contents/Plugins")
    message("INFO: ${BUNDLE_BUILTIN_PLUGINS}")

    ADD_CUSTOM_COMMAND(TARGET Centaur
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy_if_different ${CMAKE_BINARY_DIR}/lib/libBinanceSPOT.dylib ${BUNDLE_BUILTIN_PLUGINS}/libBinanceSPOT.dylib
            VERBATIM)


    ADD_CUSTOM_COMMAND(TARGET Centaur
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy_if_different ${CMAKE_BINARY_DIR}/lib/libExchangeRate.dylib ${BUNDLE_BUILTIN_PLUGINS}/libExchangeRate.dylib
            VERBATIM)
endif ()


SET(COMPILER_OPTIONS "-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-reserved-identifier -Wno-poison-system-directories")

set_source_files_properties(
        ${CENT_INCLUDE}
        PROPERTIES
        COMPILE_FLAGS "${COMPILER_OPTIONS}")

set_source_files_properties(
        ${CENT_SOURCE}
        PROPERTIES
        COMPILE_FLAGS "${COMPILER_OPTIONS}")


cmake_minimum_required(VERSION 3.14)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(cpr REQUIRED)
find_package(FMT REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(libwebsockets REQUIRED)
find_package(OpenSSL REQUIRED)


set(CMAKE_PREFIX_PATH ${CENTAUR_QT_DIRECTORY})
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


message(STATUS "LibWebSockets SPOT: libraries at ${Libwebsockets_INCLUDE_DIRS}")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)


SET(SOURCE_FILE
        src/BinanceSPOT.cpp
        src/SpotMarketWS.cpp
        ../../Library/Helpers/src/OptionsWidget.cpp
        src/StatusDialog.cpp
        src/CoinInfoDialog.cpp
        src/NetworkAddressDialog.cpp
        src/AssetDetailDialog.cpp
        src/FeesDialog.cpp
        src/BinanceSPOT_IStatus.cpp
        src/BinnaceSPOT_IBase.cpp
        src/BinanceSPOT_IExchange.cpp)

SET(INCLUDE_FILE
        include/BinanceSPOT.hpp
        ../../Library/Helpers/include/OptionsWidget.hpp
        ../../Library/Helpers/include/OptionsTableWidget.hpp
        ../../Library/Helpers/src/OptionsTableWidget.cpp
        include/CoinInfoDialog.hpp
        include/NetworkAddressDialog.hpp
        include/AssetDetailDialog.hpp
        include/FeesDialog.hpp
        include/StatusDialog.hpp
        )

SET(UI_FILES
        ui/AssetDetailDialog.ui
        ui/CoinInfoDialog.ui
        ui/FeesDialog.ui
        ui/NetworkAddress.ui
        ui/NetworksDialog.ui
        ui/StatusDialog.ui
        ../../Library/Helpers/qrc/options.qrc
        qrc/resource.qrc
        )


add_library(BinanceSPOT SHARED
        ${INCLUDE_FILE}
        ${SOURCE_FILE}
        ${UI_FILES}
        )


target_include_directories(BinanceSPOT PRIVATE
        include
        ../../Library/Exchanges/BinanceAPI/include
        ../../Library/Protocol/include
        ../include
        ../../Library/Helpers/include
        ../../include)


target_link_libraries(BinanceSPOT PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
target_link_libraries(BinanceSPOT PRIVATE fmt::fmt)
target_link_libraries(BinanceSPOT PRIVATE ${OPENSSL_LIBRARIES})
target_link_libraries(BinanceSPOT PRIVATE cpr::cpr)
target_link_libraries(BinanceSPOT PRIVATE websockets)
target_link_libraries(BinanceSPOT PRIVATE ${CMAKE_BINARY_DIR}/lib/libuuid.dylib)
target_link_libraries(BinanceSPOT PRIVATE ${CMAKE_BINARY_DIR}/lib/libProtocol.dylib)
target_link_libraries(BinanceSPOT PRIVATE ${CMAKE_BINARY_DIR}/lib/libBinanceAPI.dylib)

target_compile_definitions(BinanceSPOT PRIVATE BINANCESPOTPLUGIN_LIBRARY)


SET(COMPILER_OPTIONS "-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-reserved-identifier -Wno-poison-system-directories")

set_source_files_properties(
        ${INCLUDE_FILE}
        PROPERTIES
        COMPILE_FLAGS "${COMPILER_OPTIONS}")

set_source_files_properties(
        ${SOURCE_FILE}
        PROPERTIES
        COMPILE_FLAGS "${COMPILER_OPTIONS}")


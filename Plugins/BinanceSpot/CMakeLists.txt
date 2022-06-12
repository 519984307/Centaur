cmake_minimum_required(VERSION 3.14)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(cpr REQUIRED)
find_package(FMT REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(libwebsockets REQUIRED)
find_package(OpenSSL REQUIRED)


set(CMAKE_PREFIX_PATH /Volumes/RicardoESSD/Qt/6.3.0/macos)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


message(STATUS "LibWebSockets SPOT: libraries at ${Libwebsockets_INCLUDE_DIRS}")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)


add_library(BinanceSPOT SHARED
        qrc/resource.qrc
        src/BinanceSPOT.cpp
        include/BinanceSPOT.hpp
        src/SpotMarketWS.cpp
        # Includes for the necessary table widget
        ../../Library/Helpers/include/OptionsWidget.hpp
        ../../Library/Helpers/src/OptionsWidget.cpp
        ../../Library/Helpers/include/OptionsTableWidget.hpp
        ../../Library/Helpers/src/OptionsTableWidget.cpp
        ../../Library/Helpers/qrc/options.qrc
        include/CoinInfoDialog.hpp
        src/CoinInfoDialog.cpp
        include/NetworkAddressDialog.hpp
        src/NetworkAddressDialog.cpp)


target_include_directories(BinanceSPOT PRIVATE
        include
        ../../Library/Exchanges/BinanceAPI/include
        ../../Library/Protocol/include
        ../include
        ../../Library/Helpers/include)


target_link_libraries(BinanceSPOT PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
target_link_libraries(BinanceSPOT PRIVATE fmt::fmt)
target_link_libraries(BinanceSPOT PRIVATE ${OPENSSL_LIBRARIES})
target_link_libraries(BinanceSPOT PRIVATE cpr::cpr)
target_link_libraries(BinanceSPOT PRIVATE websockets)
target_link_libraries(BinanceSPOT PRIVATE ${CMAKE_BINARY_DIR}/lib/libuuid.dylib)
target_link_libraries(BinanceSPOT PRIVATE ${CMAKE_BINARY_DIR}/lib/libProtocol.dylib)
target_link_libraries(BinanceSPOT PRIVATE ${CMAKE_BINARY_DIR}/lib/libBinanceAPI.dylib)

target_compile_definitions(BinanceSPOT PRIVATE BINANCESPOTPLUGIN_LIBRARY)


target_compile_options(BinanceSPOT PRIVATE
        -Weverything
        -Wno-extra-semi-stmt
        -Wno-reserved-id-macro
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-c++20-compat
        -Wno-padded
        -fdiagnostics-color=always
        -Wno-poison-system-directories)


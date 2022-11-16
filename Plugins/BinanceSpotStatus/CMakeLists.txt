cmake_minimum_required(VERSION 3.14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
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

find_package(QT NAMES Qt6 COMPONENTS Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Widgets REQUIRED)
# Required libraries

SET(CPP_FILES
        include/BinanceStatus.hpp
        include/StatusDialog.hpp
        src/BinanceStatus.cpp
        src/StatusDialog.cpp)
add_library(BinanceSpotStatus SHARED
        ${CPP_FILES}
        qrc/resources.qrc
        ui/StatusDialog.ui)


target_include_directories(BinanceSpotStatus PRIVATE
        include
        ../../Library/Exchanges/BinanceAPI/include
        ../../Library/Protocol/include
        ../include
        ../../Library/Helpers/include
        ../../Library/uuid/include
        ../../include)


target_link_libraries(BinanceSpotStatus PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
target_link_libraries(BinanceSpotStatus PRIVATE fmt::fmt)
target_link_libraries(BinanceSpotStatus PRIVATE ${OPENSSL_LIBRARIES})
target_link_libraries(BinanceSpotStatus PRIVATE cpr::cpr)
target_link_libraries(BinanceSpotStatus PRIVATE websockets)
target_link_libraries(BinanceSpotStatus PRIVATE ${CMAKE_BINARY_DIR}/lib/libuuid.dylib)
target_link_libraries(BinanceSpotStatus PRIVATE ${CMAKE_BINARY_DIR}/lib/libProtocol.dylib)
target_link_libraries(BinanceSpotStatus PRIVATE ${CMAKE_BINARY_DIR}/lib/libBinanceAPI.dylib)


SET(COMPILER_OPTIONS "-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-reserved-identifier -Wno-poison-system-directories")

set_source_files_properties(
        ${CPP_FILES}
        PROPERTIES
        COMPILE_FLAGS "${COMPILER_OPTIONS}")


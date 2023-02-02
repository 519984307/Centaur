CMAKE_MINIMUM_REQUIRED(VERSION 3.24)


SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

FIND_PACKAGE(cpr REQUIRED)
FIND_PACKAGE(FMT REQUIRED)
FIND_PACKAGE(RapidJSON REQUIRED)
FIND_PACKAGE(libwebsockets REQUIRED)
FIND_PACKAGE(OpenSSL REQUIRED)


SET(Qt6_Components
        Core
        Gui
        Widgets)

IF (DEFINED CENTAUR_ENV_DETECTED)
    MESSAGE(STATUS "BinanceSPOT: Qt6 package in environment variable")
    FIND_PACKAGE(Qt6 COMPONENTS
            ${Qt6_Components}
            REQUIRED
            PATHS ${CENTAUR_ENV_DETECTED})
ELSE ()
    MESSAGE(STATUS "BinanceSPOT: Qt6 package in default paths")
    FIND_PACKAGE(Qt6 COMPONENTS
            ${Qt6_Components}
            REQUIRED)
ENDIF ()

SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)


SET(SOURCE_FILES
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

SET(INCLUDE_FILES
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


ADD_LIBRARY(BinanceSPOT SHARED
        ${INCLUDE_FILE}
        ${SOURCE_FILE}
        ${UI_FILES}
        )


TARGET_INCLUDE_DIRECTORIES(BinanceSPOT PRIVATE
        include
        ../../Library/Exchanges/BinanceAPI/include
        ../../Library/Protocol/include
        ../include
        ../../Library/Helpers/include
        ../../include)


TARGET_LINK_LIBRARIES(BinanceSPOT PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
TARGET_LINK_LIBRARIES(BinanceSPOT PRIVATE fmt::fmt)
TARGET_LINK_LIBRARIES(BinanceSPOT PRIVATE ${OPENSSL_LIBRARIES})
TARGET_LINK_LIBRARIES(BinanceSPOT PRIVATE cpr::cpr)
TARGET_LINK_LIBRARIES(BinanceSPOT PRIVATE websockets)
TARGET_LINK_LIBRARIES(BinanceSPOT PRIVATE ${CMAKE_BINARY_DIR}/lib/libuuid.dylib)
TARGET_LINK_LIBRARIES(BinanceSPOT PRIVATE ${CMAKE_BINARY_DIR}/lib/libProtocol.dylib)
TARGET_LINK_LIBRARIES(BinanceSPOT PRIVATE ${CMAKE_BINARY_DIR}/lib/libBinanceAPI.dylib)

TARGET_COMPILE_DEFINITIONS(BinanceSPOT PRIVATE BINANCESPOTPLUGIN_LIBRARY)


IF (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Control clang warnings
    SET(COMPILER_OPTIONS " -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-reserved-identifier -Wno-poison-system-directories ")

    SET_SOURCE_FILES_PROPERTIES(
            ${INCLUDE_FILES}
            PROPERTIES
            COMPILE_FLAGS "${COMPILER_OPTIONS}")

    SET_SOURCE_FILES_PROPERTIES(
            ${SOURCE_FILES}
            PROPERTIES
            COMPILE_FLAGS "${COMPILER_OPTIONS}")


    MESSAGE(STATUS "libBinanceSPOT: Clang detected - Warning Level Set")
ENDIF ()



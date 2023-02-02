cmake_minimum_required(VERSION 3.24)


SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

FIND_PACKAGE(FMT REQUIRED)
FIND_PACKAGE(RapidJSON REQUIRED)
FIND_PACKAGE(SQLite3 REQUIRED)
FIND_PACKAGE(libwebsockets REQUIRED)
FIND_PACKAGE(OpenSSL REQUIRED)
FIND_PACKAGE(re2)

IF (UNIX AND NOT APPLE)
    ADD_COMPILE_DEFINITIONS(P_LINUX)
    MESSAGE(STATUS "BTRADER: Linux Platform detected")
ELSEIF (WIN32)
    ADD_COMPILE_DEFINITIONS(P_WINDOWS)
    MESSAGE(STATUS "BTRADER: Windows Platform detected")
ELSEIF (UNIX AND APPLE)
    ADD_COMPILE_DEFINITIONS(P_APPLE)
    MESSAGE(STATUS "BTRADER: Apple Platforms detected")
    SET(PLATFORM_INDEPENDENT_FILES
            src/apple/AppleDirectory.mm
            include/apple/AppleDirectory.h
            src/SpotStreams.cpp)
ELSE ()
    MESSAGE(FATAL_ERROR "BTRADER: Unknown target platform")
ENDIF ()


SET(INCLUDE_FILES
        include/TraderApplication.hpp
        include/ClientSocket.hpp
        include/TraderGlobals.hpp
        include/ApplicationMessages.hpp
        include/ui/Icons.hpp
        ../common/ToolsThread.hpp
        ../common/ToolsLogger.hpp
        ../common/Tools.hpp
        include/SpotStreams.hpp)

SET(SOURCE_FILES
        src/main.cpp
        src/KeysLoader.cpp
        src/TraderApplication.cpp
        src/ClientSocket.cpp)

ADD_EXECUTABLE(btrader
        ${PLATFORM_INDEPENDENT_FILES}
        ${SOURCE_FILES}
        ${INCLUDE_FILES})


TARGET_INCLUDE_DIRECTORIES(btrader PRIVATE
        include
        ../common
        ../../Library/Protocol/include
        ../../Library/uuid/include
        ../../Library/Exchanges/BinanceAPI/include
        ../../include)


TARGET_LINK_LIBRARIES(btrader PRIVATE fmt::fmt)
TARGET_LINK_LIBRARIES(btrader PRIVATE SQLite3)
TARGET_LINK_LIBRARIES(btrader PRIVATE ${OPENSSL_LIBRARIES})
TARGET_LINK_LIBRARIES(btrader PRIVATE websockets)
TARGET_LINK_LIBRARIES(btrader PRIVATE re2::re2)

# Local Libraries
TARGET_LINK_LIBRARIES(btrader PRIVATE ${CMAKE_BINARY_DIR}/lib/libProtocol.dylib)
TARGET_LINK_LIBRARIES(btrader PRIVATE ${CMAKE_BINARY_DIR}/lib/libuuid.dylib)
TARGET_LINK_LIBRARIES(btrader PRIVATE ${CMAKE_BINARY_DIR}/lib/libBinanceAPI.dylib)


IF (UNIX AND APPLE)
    TARGET_LINK_LIBRARIES(btrader PRIVATE "-framework Foundation")
ENDIF ()


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


    MESSAGE(STATUS "btrader: Clang detected - Warning Level Set")
ENDIF ()



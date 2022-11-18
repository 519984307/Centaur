cmake_minimum_required(VERSION 3.14)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(FMT REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(libwebsockets REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(re2)

IF (UNIX AND NOT APPLE)
    add_compile_definitions(P_LINUX)
    message("Linux Platform detected")
ELSEIF (WIN32)
    add_compile_definitions(P_WINDOWS)
    message("Windows Platform detected")
ELSEIF (APPLE)
    add_compile_definitions(P_APPLE)
    message("Apple Platforms detected")
    set(PLATFORM_INDEPENDENT_FILES
            src/apple/AppleDirectory.mm
            include/apple/AppleDirectory.h
            src/SpotStreams.cpp)
ELSE ()
    message(FATAL_ERROR "Unknown target platform")
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

SET(SOURCE_FILE
        src/main.cpp
        src/KeysLoader.cpp
        src/TraderApplication.cpp
        src/ClientSocket.cpp)

add_executable(btrader
        ${PLATFORM_INDEPENDENT_FILES}
        ${SOURCE_FILE}
        ${INCLUDE_FILES})


target_include_directories(btrader PRIVATE
        include
        ../common
        ../../Library/Protocol/include
        ../../Library/uuid/include
        ../../Library/Exchanges/BinanceAPI/include
        ../../include)


target_link_libraries(btrader PRIVATE fmt::fmt)
target_link_libraries(btrader PRIVATE SQLite3)
target_link_libraries(btrader PRIVATE ${OPENSSL_LIBRARIES})
target_link_libraries(btrader PRIVATE websockets)
target_link_libraries(btrader PRIVATE re2::re2)

# Local Libraries
target_link_libraries(btrader PRIVATE ${CMAKE_BINARY_DIR}/lib/libProtocol.dylib)
target_link_libraries(btrader PRIVATE ${CMAKE_BINARY_DIR}/lib/libuuid.dylib)
target_link_libraries(btrader PRIVATE ${CMAKE_BINARY_DIR}/lib/libBinanceAPI.dylib)


IF (APPLE)
    target_link_libraries(btrader PRIVATE "-framework Foundation")
ENDIF ()


set_source_files_properties(
        ${CENT_INCLUDE}
        PROPERTIES
        COMPILE_FLAGS "${COMPILER_OPTIONS}")

set_source_files_properties(
        ${CENT_SOURCE}
        PROPERTIES
        COMPILE_FLAGS "${COMPILER_OPTIONS}")

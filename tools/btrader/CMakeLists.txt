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
            src/AppleDirectory.mm
            include/AppleDirectory.h
            )
ELSE ()
    message(FATAL_ERROR "Unknown target platform")
ENDIF ()

add_executable(btrader
        ${PLATFORM_INDEPENDENT_FILES}
        src/main.cpp
        src/KeysLoader.cpp
        src/TraderApplication.cpp
        src/ClientSocket.cpp
        include/TraderApplication.hpp
        include/ClientSocket.hpp
        include/Globals.hpp
        ../common/ToolsThread.hpp
        ../common/ToolsLogger.hpp
        ../common/Tools.hpp
        )

target_include_directories(btrader PRIVATE
        include
        ../common
        ../../Library/Protocol/include
        ../../Library/uuid/include
        ../../Library/Exchanges/BinanceAPI/include)


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


target_compile_options(btrader PRIVATE
        -Weverything
        -Wno-extra-semi-stmt
        -Wno-reserved-id-macro
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-c++20-compat
        -Wno-padded
        -fdiagnostics-color=always
        -Wno-poison-system-directories)


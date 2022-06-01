cmake_minimum_required(VERSION 3.14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(cpr REQUIRED)
find_package(FMT CONFIG REQUIRED)
find_package(RapidJSON REQUIRED)

set(CMAKE_PREFIX_PATH "/Volumes/RicardoESSD/Qt/6.3.0/macos")
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Gui LinguistTools Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui LinguistTools Widgets REQUIRED)
# Required libraries


add_library(ExchangeRate SHARED
        include/ExchangeRate.hpp
        src/ExchangeRate.cpp)


target_include_directories(ExchangeRate PRIVATE
        include
        ../include
        ../../Library/Protocol/include
        ../../Library/uuid/include)


target_link_libraries(ExchangeRate PRIVATE Qt${QT_VERSION_MAJOR}::Gui)
target_link_libraries(ExchangeRate PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(ExchangeRate PRIVATE fmt::fmt)
target_link_libraries(ExchangeRate PRIVATE cpr::cpr)

target_link_libraries(ExchangeRate PRIVATE ${CMAKE_BINARY_DIR}/lib/libuuid.dylib)
target_link_libraries(ExchangeRate PRIVATE ${CMAKE_BINARY_DIR}/lib/libProtocol.dylib)


target_compile_options(ExchangeRate PRIVATE
        -Weverything
        -Wno-extra-semi-stmt
        -Wno-reserved-id-macro
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-c++20-compat
        -Wno-padded
        -fdiagnostics-color=always
        -Wno-poison-system-directories)


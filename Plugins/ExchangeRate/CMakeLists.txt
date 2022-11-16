cmake_minimum_required(VERSION 3.14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(cpr REQUIRED)
find_package(FMT CONFIG REQUIRED)
find_package(RapidJSON REQUIRED)

set(CMAKE_PREFIX_PATH ${CENTAUR_QT_DIRECTORY})
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Gui LinguistTools Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui LinguistTools Widgets REQUIRED)
# Required libraries

SET(PROJECT_FILES
        include/ExchangeRate.hpp
        src/ExchangeRate.cpp
        res/resources.qrc)

add_library(ExchangeRate SHARED
        ${PROJECT_FILES}
        )


target_include_directories(ExchangeRate PRIVATE
        include
        ../include
        ../../Library/Protocol/include
        ../../Library/uuid/include
        ../../include/)


target_link_libraries(ExchangeRate PRIVATE Qt${QT_VERSION_MAJOR}::Gui)
target_link_libraries(ExchangeRate PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(ExchangeRate PRIVATE fmt::fmt)
target_link_libraries(ExchangeRate PRIVATE cpr::cpr)

target_link_libraries(ExchangeRate PRIVATE ${CMAKE_BINARY_DIR}/lib/libuuid.dylib)
target_link_libraries(ExchangeRate PRIVATE ${CMAKE_BINARY_DIR}/lib/libProtocol.dylib)


SET(COMPILER_OPTIONS "-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-reserved-identifier -Wno-poison-system-directories")

set_source_files_properties(
        ${PROJECT_FILES}
        PROPERTIES
        COMPILE_FLAGS "${COMPILER_OPTIONS}")

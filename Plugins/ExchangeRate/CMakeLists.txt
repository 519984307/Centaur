cmake_minimum_required(VERSION 3.14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/Volumes/RicardoESSD/Qt/6.2.1/macos")


#set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/local/opt/openssl/lib)
#set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} /usr/local/opt/openssl/include)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Gui LinguistTools Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui LinguistTools Widgets REQUIRED)
# Required libraries
find_package(fmt REQUIRED)

add_library(ExchangeRate SHARED
        include/ExchangeRate.hpp
        src/ExchangeRate.cpp)


target_include_directories(ExchangeRate PRIVATE
        include
        ../include
        /usr/local/Cellar/rapidjson/1.1.0/include
        /usr/local/opt/cpr/include)


target_link_libraries(ExchangeRate PRIVATE Qt${QT_VERSION_MAJOR}::Gui)
target_link_libraries(ExchangeRate PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(ExchangeRate PRIVATE fmt::fmt)
target_link_libraries(ExchangeRate PRIVATE /usr/local/opt/cpr/lib/libcpr.dylib)


target_compile_definitions(ExchangeRate PRIVATE EXCHANGERATEPLUGIN_LIBRARY)

if (COMMAND qt_create_translation)
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else ()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif ()


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


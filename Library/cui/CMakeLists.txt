cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_COLOR_DIAGNOSTICS ON)

set(CMAKE_PREFIX_PATH ${CENTAUR_QT_DIRECTORY})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Sql
        Concurrent
        REQUIRED)


set(INCLUDE_FILES
        include/EditorDelegate.hpp
        include/OptionsWidget.hpp
        include/OptionsTableWidget.hpp
        include/ResizableFrame.hpp
        include/MovableFrame.hpp
        include/SystemPushButton.hpp
        include/LinkedLineEdit.hpp
        include/PasswordLineEdit.hpp
        include/SearchLineEdit.hpp
        include/UserLineEdit.hpp
        include/CentaurLibrary.hpp
        include/ChevronButton.hpp
        include/AnimatedButton.hpp
        include/SquarifyWidget.hpp
        include/CandleItem.hpp
        include/CandleChartWidget.hpp
        include/CandleChartScene.hpp
        include/CandlePriceAxisItem.hpp
        include/CandleTimeAxisItem.hpp
        )

set(SOURCE_FILES
        src/EditorDelegate.cpp
        src/OptionsWidget.cpp
        src/OptionsTableWidget.cpp
        src/ResizableFrame.cpp
        src/MovableFrame.cpp
        src/SystemPushButton.cpp
        src/LinkedLineEdit.cpp
        src/PasswordLineEdit.cpp
        src/UserLineEdit.cpp
        src/SearchLineEdit.cpp
        src/ChevronButton.cpp
        src/AnimatedButton.cpp
        src/SquarifyWidget.cpp
        src/CandleItem.cpp
        src/CandleChartWidget.cpp
        src/CandleChartScene.cpp
        src/CandlePriceAxisItem.cpp
        src/CandleTimeAxisItem.cpp)

set(UI_FILES
        rcc/resources.qrc
        ui/OptionsWidget.ui)

set(LIBCUI cui)

IF (UNIX AND NOT APPLE)
    message(STATUS "Centaur Linux Detected")
ELSEIF (WIN32)
    message(STATUS "Centaur Windows Detected")
    set(CENTAUR_EXECUTABLE WIN32)
ELSEIF (UNIX AND APPLE)
    message(STATUS "Centaur Apple Detected")
ELSE ()
    message(FATAL_ERROR "Centaur Unknown target platform")
ENDIF ()


add_library(${LIBCUI} SHARED
        ${INCLUDE_FILES}
        ${SOURCE_FILES}
        ${UI_FILES})

target_compile_definitions(${LIBCUI} PRIVATE CENTFIN_LIBRARY_)

target_include_directories(${LIBCUI} PRIVATE ../../include)
target_include_directories(${LIBCUI} PRIVATE include)

target_link_libraries(${LIBCUI} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql Qt6::Concurrent)


SET(COMPILER_OPTIONS "-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-reserved-identifier -Wno-poison-system-directories")

set_source_files_properties(
        ${INCLUDE_FILES}
        PROPERTIES
        COMPILE_FLAGS "${COMPILER_OPTIONS}")

set_source_files_properties(
        ${SOURCE_FILES}
        PROPERTIES
        COMPILE_FLAGS "${COMPILER_OPTIONS}")



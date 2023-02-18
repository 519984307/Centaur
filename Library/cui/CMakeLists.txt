CMAKE_MINIMUM_REQUIRED(VERSION 3.25)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_COLOR_DIAGNOSTICS ON)

SET(Qt6_Components
        Core
        Gui
        Widgets
        Sql
        Concurrent)

IF (DEFINED CENTAUR_ENV_DETECTED)
    MESSAGE(STATUS "CUI: Qt6 package in environment variable")
    FIND_PACKAGE(Qt6 COMPONENTS
            ${Qt6_Components}
            REQUIRED
            PATHS ${CENTAUR_ENV_DETECTED} NO_DEFAULT_PATH)
ELSE ()
    MESSAGE(STATUS "CUI: Qt6 package in default paths")
    FIND_PACKAGE(Qt6 COMPONENTS
            ${Qt6_Components}
            REQUIRED)
ENDIF ()


SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)


SET(INCLUDE_FILES
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

SET(SOURCE_FILES
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

SET(UI_FILES
        rcc/resources.qrc
        ui/OptionsWidget.ui)

SET(LIBCUI cui)

IF (UNIX AND NOT APPLE)
    MESSAGE(STATUS "CUI: Linux Detected")
ELSEIF (WIN32)
    MESSAGE(STATUS "CUI: Windows Detected")
    SET(CENTAUR_EXECUTABLE WIN32)
ELSEIF (UNIX AND APPLE)
    MESSAGE(STATUS "CUI: Apple Detected")
ELSE ()
    MESSAGE(FATAL_ERROR "CUI: Unknown target platform")
ENDIF ()


ADD_LIBRARY(${LIBCUI} SHARED
        ${INCLUDE_FILES}
        ${SOURCE_FILES}
        ${UI_FILES})

TARGET_COMPILE_DEFINITIONS(${LIBCUI} PRIVATE CENTFIN_LIBRARY_)

TARGET_INCLUDE_DIRECTORIES(${LIBCUI} PRIVATE ../../include)
TARGET_INCLUDE_DIRECTORIES(${LIBCUI} PRIVATE include)

TARGET_LINK_LIBRARIES(${LIBCUI} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql Qt6::Concurrent)


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


    MESSAGE(STATUS "libCUI: Clang detected - Warning Level Set")
ENDIF ()




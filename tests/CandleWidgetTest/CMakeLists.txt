CMAKE_MINIMUM_REQUIRED(VERSION 3.14)


SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(Qt6_Components
        Core
        Gui
        Widgets
        Charts
        Sql
        WebSockets)

IF (DEFINED CENTAUR_ENV_DETECTED)
    MESSAGE(STATUS "CandleWidgetTests: Qt6 package in environment variable")
    FIND_PACKAGE(Qt6 COMPONENTS
            ${Qt6_Components}
            REQUIRED
            PATHS ${CENTAUR_ENV_DETECTED})
ELSE ()
    MESSAGE(STATUS "CandleWidgetTests: Qt6 package in default paths")
    FIND_PACKAGE(Qt6 COMPONENTS
            ${Qt6_Components}
            REQUIRED)
ENDIF ()

SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)


SET(SOURCE_FILES
        main.cpp TestDialog.cpp TestData.cpp)

SET(INCLUDE_FILES
        TestDialog.hpp)

SET(UI_FILES
        )


ADD_EXECUTABLE(CandleWidgetTest
        ${INCLUDE_FILSE}
        ${SOURCE_FILES}
        ${UI_FILES}
        )


TARGET_INCLUDE_DIRECTORIES(CandleWidgetTest PRIVATE
        ../../Library/cui/include
        ../../Plugins/include
        ../../include)


TARGET_LINK_LIBRARIES(CandleWidgetTest PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
TARGET_LINK_LIBRARIES(CandleWidgetTest PRIVATE ${CMAKE_BINARY_DIR}/lib/libcui.dylib)


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


    MESSAGE(STATUS "CandleWidgetTests: Clang detected - Warning Level Set")
ENDIF ()


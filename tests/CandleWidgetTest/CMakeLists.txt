cmake_minimum_required(VERSION 3.14)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_PREFIX_PATH ${CENTAUR_QT_DIRECTORY})
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)


SET(SOURCE_FILE
        main.cpp TestDialog.cpp TestData.cpp)

SET(INCLUDE_FILE
        TestDialog.hpp)

SET(UI_FILES
        )


add_executable(CandleWidgetTest
        ${INCLUDE_FILE}
        ${SOURCE_FILE}
        ${UI_FILES}
        )


target_include_directories(CandleWidgetTest PRIVATE
        ../../Library/cui/include
        ../../Plugins/include
        ../../include)


target_link_libraries(CandleWidgetTest PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
target_link_libraries(CandleWidgetTest PRIVATE ${CMAKE_BINARY_DIR}/lib/libcui.dylib)


SET(COMPILER_OPTIONS "-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-reserved-identifier -Wno-poison-system-directories")

set_source_files_properties(
        ${INCLUDE_FILE}
        PROPERTIES
        COMPILE_FLAGS "${COMPILER_OPTIONS}")

set_source_files_properties(
        ${SOURCE_FILE}
        PROPERTIES
        COMPILE_FLAGS "${COMPILER_OPTIONS}")


cmake_minimum_required(VERSION 3.21)
project(Centaur)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/Volumes/RicardoESSD/Qt/6.2.1/macos")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(Centaur main.cpp)
target_link_libraries(Centaur
        Qt::Core
        Qt::Gui
        Qt::Widgets
        )


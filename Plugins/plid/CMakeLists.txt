cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 23)

find_package(FMT REQUIRED)
find_package(rapidjson REQUIRED)

add_executable(plid main.cpp)


target_include_directories(plid PRIVATE
        ../../Centaur/include
        "${RAPIDJSON_INCLUDE_DIRS}")

target_link_libraries(plid PRIVATE fmt::fmt)


target_compile_options(plid PRIVATE
        -Weverything
        -Wno-extra-semi-stmt
        -Wno-reserved-id-macro
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-c++20-compat
        -Wno-padded
        -Wno-poison-system-directories
        -fdiagnostics-color=always)

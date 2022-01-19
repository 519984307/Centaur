cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)


add_library(uuid
        SHARED
        include/uuid.hpp
        src/uuid.cpp)

target_compile_definitions(uuid PRIVATE BINAPI_LIBRARY)

include_directories(include PRIVATE)


target_compile_options(uuid PRIVATE
        -Weverything
        -Wno-extra-semi-stmt
        -Wno-reserved-id-macro
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-c++20-compat
        -Wno-padded
        -Wno-poison-system-directories
        -fdiagnostics-color=always)

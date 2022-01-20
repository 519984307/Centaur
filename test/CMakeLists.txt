cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)

find_package(Catch2 CONFIG REQUIRED)

message("Output dir: ${CMAKE_BINARY_DIR}")

add_executable(tests
        main.cpp
        tests.cpp)


target_include_directories(tests PRIVATE
        ../Library/uuid/include)

target_link_libraries(tests PRIVATE Catch2::Catch2)


target_compile_options(tests PRIVATE
        -Weverything
        -Wno-extra-semi-stmt
        -Wno-reserved-id-macro
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-c++20-compat
        -Wno-padded
        -Wno-poison-system-directories
        -fdiagnostics-color=always)

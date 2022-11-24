cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 20)

find_package(Catch2 CONFIG REQUIRED)


add_executable(tests
        main.cpp
        tests.cpp)


target_include_directories(tests PRIVATE
        ../Library/uuid/include
        ../Library/Protocol/include
        ../include)

IF (UNIX AND NOT APPLE)
    message("Centaur Linux Detected")
ELSEIF (WIN32)
    message("Centaur Windows Detected")
    set(CENTAUR_EXECUTABLE WIN32)
ELSEIF (UNIX AND APPLE)
    message("Centaur Apple Detected")
    set(CENTAUR_EXECUTABLE MACOSX_BUNDLE)
    set(CMAKE_MACOSX_RPATH 1)
    set(CMAKE_INSTALL_RPATH "${CMAKE_BINARY_DIR}/lib")
ELSE ()
    message(FATAL_ERROR "Unknown target platform")
ENDIF ()


target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(tests PRIVATE ${CMAKE_BINARY_DIR}/lib/libProtocol.dylib)
target_link_libraries(tests PRIVATE ${CMAKE_BINARY_DIR}/lib/libuuid.dylib)


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

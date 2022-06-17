cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)

# Required libraries
find_package(fmt REQUIRED)
find_package(RapidJson REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)


add_library(protocol SHARED
        src/protocol.cpp
        include/protocol.hpp
        ../../Centaur.hpp
        src/generator.cpp
        src/encryption.cpp)

include_directories(include PRIVATE)


target_link_libraries(protocol PRIVATE fmt::fmt)
target_link_libraries(protocol PRIVATE ${OPENSSL_LIBRARIES})
target_link_libraries(protocol PRIVATE ZLIB::ZLIB)


target_compile_options(protocol PRIVATE
        -Weverything
        -Wno-extra-semi-stmt
        -Wno-reserved-id-macro
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-c++20-compat
        -Wno-padded
        -Wno-poison-system-directories
        -fdiagnostics-color=always
        )

cmake_minimum_required(VERSION 3.24)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_COLOR_DIAGNOSTICS ON)


# Required libraries
find_package(fmt REQUIRED)
find_package(RapidJson REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

SET(INCLUDE_FILES
        ../../include/Centaur.hpp
        include/Protocol.hpp
        include/ProtocolType.hpp)

SET(SOURCE_FILES
        src/protocol.cpp
        src/generator.cpp
        src/encryption.cpp)

ADD_LIBRARY(Protocol SHARED
        ${INCLUDE_FILES}
        ${SOURCE_FILES})

TARGET_INCLUDE_DIRECTORIES(Protocol PRIVATE
        include
        ../../include)


TARGET_LINK_LIBRARIES(Protocol PRIVATE fmt::fmt)
TARGET_LINK_LIBRARIES(Protocol PRIVATE OpenSSL::SSL OpenSSL::Crypto)
TARGET_LINK_LIBRARIES(Protocol PRIVATE ZLIB::ZLIB)


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


    MESSAGE(STATUS "libProtocol: Clang detected - Warning Level Set")
ENDIF ()

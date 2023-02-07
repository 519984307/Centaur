cmake_minimum_required(VERSION 3.24)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

FIND_PACKAGE(FMT REQUIRED)
FIND_PACKAGE(RapidJSON REQUIRED)
FIND_PACKAGE(OpenSSL REQUIRED)
FIND_PACKAGE(CLI11 CONFIG REQUIRED)
FIND_PACKAGE(libzip CONFIG REQUIRED)


IF (UNIX AND NOT APPLE)
    ADD_COMPILE_DEFINITIONS(P_LINUX)
    MESSAGE(STATUS "plpack: Linux Platform detected")
ELSEIF (WIN32)
    ADD_COMPILE_DEFINITIONS(P_WINDOWS)
    MESSAGE(STATUS "plpack: Windows Platform detected")
ELSEIF (UNIX AND APPLE)
    ADD_COMPILE_DEFINITIONS(P_APPLE)
    MESSAGE(STATUS "plpack: Apple Platforms detected")
ELSE ()
    MESSAGE(FATAL_ERROR "plpack: Unknown target platform")
ENDIF ()


SET(SOURCE_FILES
        main.cpp)

ADD_EXECUTABLE(plpack
        ${SOURCE_FILES})


TARGET_LINK_LIBRARIES(plpack PRIVATE fmt::fmt)
TARGET_LINK_LIBRARIES(plpack PRIVATE OpenSSL::SSL OpenSSL::Crypto)
TARGET_LINK_LIBRARIES(plpack PRIVATE CLI11::CLI11)
TARGET_LINK_LIBRARIES(plpack PRIVATE libzip::zip)

# Local Libraries
TARGET_LINK_LIBRARIES(plpack PRIVATE ${CMAKE_BINARY_DIR}/lib/libProtocol.dylib)
TARGET_LINK_LIBRARIES(plpack PRIVATE ${CMAKE_BINARY_DIR}/lib/libuuid.dylib)
TARGET_LINK_LIBRARIES(plpack PRIVATE ${CMAKE_BINARY_DIR}/lib/libBinanceAPI.dylib)


IF (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Control clang warnings
    SET(COMPILER_OPTIONS " -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-reserved-identifier -Wno-poison-system-directories -Wno-nullable-to-nonnull-conversion")


    SET_SOURCE_FILES_PROPERTIES(
            ${SOURCE_FILES}
            PROPERTIES
            COMPILE_FLAGS "${COMPILER_OPTIONS}")


    MESSAGE(STATUS "plpack: Clang detected - Warning Level Set")
ENDIF ()



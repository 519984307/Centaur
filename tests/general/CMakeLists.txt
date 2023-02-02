CMAKE_MINIMUM_REQUIRED(VERSION 3.24)

SET(CMAKE_CXX_STANDARD 20)

FIND_PACKAGE(Catch2 CONFIG REQUIRED)


IF (UNIX AND NOT APPLE)
    MESSAGE(STATUS "TESTS: Linux Detected")
ELSEIF (WIN32)
    MESSAGE(STATUS "TESTS: Windows Detected")
    SET(CENTAUR_EXECUTABLE WIN32)
ELSEIF (UNIX AND APPLE)
    MESSAGE(STATUS "TESTS: Apple Detected")
    SET(CMAKE_MACOSX_RPATH 1)
    SET(CMAKE_INSTALL_RPATH "${CMAKE_BINARY_DIR}/lib")
ELSE ()
    message(FATAL_ERROR "TESTS: Unknown target platform")
ENDIF ()

SET(SOURCE_FILES main.cpp tests.cpp)

ADD_EXECUTABLE(tests
        ${SOURCE_FILES})


TARGET_INCLUDE_DIRECTORIES(tests PRIVATE
        ../../Library/uuid/include
        ../../Library/Protocol/include
        ../../include)


TARGET_LINK_LIBRARIES(tests PRIVATE Catch2::Catch2WithMain)
TARGET_LINK_LIBRARIES(tests PRIVATE ${CMAKE_BINARY_DIR}/lib/libProtocol.dylib)
TARGET_LINK_LIBRARIES(tests PRIVATE ${CMAKE_BINARY_DIR}/lib/libuuid.dylib)


IF (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Control clang warnings
    SET(COMPILER_OPTIONS " -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-reserved-identifier -Wno-poison-system-directories ")

    SET_SOURCE_FILES_PROPERTIES(
            ${SOURCE_FILES}
            PROPERTIES
            COMPILE_FLAGS "${COMPILER_OPTIONS}")


    MESSAGE(STATUS "Tests: Clang detected - Warning Level Set")
ENDIF ()


CMAKE_MINIMUM_REQUIRED(VERSION 3.24)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_COLOR_DIAGNOSTICS ON)


# required libraries
FIND_PACKAGE(fmt REQUIRED)
FIND_PACKAGE(rapidjson REQUIRED)
FIND_PACKAGE(cpr CONFIG REQUIRED)
FIND_PACKAGE(libwebsockets CONFIG REQUIRED)
FIND_PACKAGE(OpenSSL REQUIRED)

SET(INCLUDE_FILES
        include/apiexception.hpp
        include/binapi.hpp
        include/binanceapidefs.hpp
        include/binanceapiglobal.hpp
        include/binanceapilimits.hpp
        include/schemas.hpp
        include/wsspotbinanceapi.hpp
        include/wsfuturesbinanceapi.hpp
        include/wsfuturesbinanceuser.hpp
        include/binanceapi.hpp
        include/wsthread.hpp)

SET(SOURCE_FILES
        src/apiexception.cpp
        src/binapi.cpp
        src/binanceapilimits.cpp
        src/schemas.cpp
        src/wsfuturesbinanceapi.cpp
        src/wsfuturesbinanceuser.cpp
        src/wsspotbinanceapi.cpp
        src/wsthread.cpp
        src/binanceapispot.cpp)

ADD_LIBRARY(BinanceAPI SHARED
        ${INCLUDE_FILES}
        ${SOURCE_FILES})


TARGET_INCLUDE_DIRECTORIES(BinanceAPI PRIVATE include)


TARGET_LINK_LIBRARIES(BinanceAPI PRIVATE fmt::fmt)
TARGET_LINK_LIBRARIES(BinanceAPI PRIVATE cpr::cpr)
TARGET_LINK_LIBRARIES(BinanceAPI PRIVATE OpenSSL::SSL OpenSSL::Crypto)
TARGET_LINK_LIBRARIES(BinanceAPI PRIVATE websockets)
TARGET_LINK_LIBRARIES(BinanceAPI PRIVATE rapidjson)


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


    MESSAGE(STATUS "libBinanceAPI: Clang detected - Warning Level Set")
ENDIF ()

cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)


# OpenSSL Data
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/local/opt/openssl/lib)
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} /usr/local/opt/openssl/include)

# Required libraries
find_package(OpenSSL REQUIRED)
find_package(FMT REQUIRED)
find_package(ZLIB REQUIRED)
find_package(CURL REQUIRED)
find_package(libwebsockets REQUIRED)

add_library(BinanceAPI SHARED
        include/APIException.hpp
        include/Binapi.hpp
        include/BinanceAPIDefs.hpp
        include/BinanceAPIGlobal.hpp
        include/BinanceAPILimits.hpp
        include/schemas.hpp
        include/WSSpotBinanceAPI.hpp
        include/WSFuturesBinanceAPI.hpp
        include/WSFuturesBinanceUser.hpp
        src/APIException.cpp
        src/Binapi.cpp
        src/BinanceAPILimits.cpp
        src/schemas.cpp
        src/WSFuturesBinanceAPI.cpp
        src/WSFuturesBinanceUser.cpp
        src/WSSpotBinanceAPI.cpp
        include/BinanceAPI.hpp include/BinanceWS.hpp src/BinanceWS.cpp)

target_compile_definitions(BinanceAPI PRIVATE BINAPI_LIBRARY)


include_directories(include PRIVATE)
include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(/usr/local/opt/cpr/include)
include_directories(/usr/local/Cellar/rapidjson/1.1.0/include)

target_include_directories(BinanceAPI PUBLIC ${OPENSSL_INCLUDE_DIR})


target_link_libraries(BinanceAPI PRIVATE fmt::fmt)
target_link_libraries(BinanceAPI PRIVATE curl)
target_link_libraries(BinanceAPI PRIVATE ${OPENSSL_LIBRARIES})
target_link_libraries(BinanceAPI PRIVATE /usr/local/opt/cpr/lib/libcpr.dylib)
target_link_libraries(BinanceAPI PRIVATE ${ZLIB_LIBRARIES})
target_link_libraries(BinanceAPI PRIVATE /usr/local/opt/libwebsockets/lib/libwebsockets.dylib)


target_compile_options(BinanceAPI PRIVATE
        -Weverything
        -Wno-extra-semi-stmt
        -Wno-reserved-id-macro
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-c++20-compat
        -Wno-padded
        -Wno-poison-system-directories
        -fdiagnostics-color=always)

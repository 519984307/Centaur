cmake_minimum_required(VERSION 3.21)


# Required libraries
find_package(FMT REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(cpr REQUIRED)
find_package(libwebsockets REQUIRED)
find_package(OpenSSL REQUIRED)


message(STATUS "OpenSSL: Version ${OPENSSL_VERSION}")
message(STATUS "OpenSSL: include dir at ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL: libraries at ${OPENSSL_LIBRARIES} ${OPENSSL_SSL_LIBRARIES}")


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
        include/BinanceAPI.hpp
        include/WSThread.hpp
        src/APIException.cpp
        src/Binapi.cpp
        src/BinanceAPILimits.cpp
        src/schemas.cpp
        src/WSFuturesBinanceAPI.cpp
        src/WSFuturesBinanceUser.cpp
        src/WSSpotBinanceAPI.cpp
        src/WSThread.cpp
        src/BinanceAPISpot.cpp)


include_directories(include PRIVATE)


target_link_libraries(BinanceAPI PRIVATE fmt::fmt)
target_link_libraries(BinanceAPI PRIVATE cpr::cpr)
target_link_libraries(BinanceAPI PRIVATE ${OPENSSL_LIBRARIES})
target_link_libraries(BinanceAPI PRIVATE websockets)


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

cmake_minimum_required(VERSION 3.24)

PROJECT(Centaur VERSION 1.0
        DESCRIPTION "Centaur. Trading Application"
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_COLOR_DIAGNOSTICS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


set(CENTAUR_QT_DIRECTORY PUBLIC "/Volumes/RicardoESSD/Qt/6.3.0/macos")

include_directories(Plugins/include)

# Main Application
add_subdirectory(Centaur)

#Clients

#Libraries
add_subdirectory(Library/uuid)
add_subdirectory(Library/Protocol)

#Libraries for exchanges
add_subdirectory(Library/Exchanges/BinanceAPI)

#Plugins
add_subdirectory(Plugins/ExchangeRate)
add_subdirectory(Plugins/BinanceSpot)
add_subdirectory(Plugins/BinanceSpotStatus)
add_subdirectory(Plugins/BinanceCS)

#tests
add_subdirectory(test)

#Tools
add_subdirectory(tools/imgSorter)
add_subdirectory(tools/btrader)

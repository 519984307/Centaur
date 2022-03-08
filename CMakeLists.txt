cmake_minimum_required(VERSION 3.21)

PROJECT(Centaur VERSION 1.0
        DESCRIPTION "Centaur. Trading Application"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


include_directories(Plugins/include)

# Main Application
add_subdirectory(Centaur)

#Clients

#Libraries
add_subdirectory(Library/uuid)
add_subdirectory(Library/Protocol)

#Plugins
add_subdirectory(Plugins/ExchangeRate)
add_subdirectory(Plugins/BinanceSpot)

#tests
add_subdirectory(test)


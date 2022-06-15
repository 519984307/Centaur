

# Conan automatically generated toolchain file
# DO NOT EDIT MANUALLY, it will be overwritten

# Avoid including toolchain file several times (bad if appending to variables like
#   CMAKE_CXX_FLAGS. See https://github.com/android/ndk/issues/323
include_guard()

message(STATUS "Using Conan toolchain: ${CMAKE_CURRENT_LIST_FILE}")

if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeToolchain' generator only works with CMake >= 3.15")
endif()





set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)


# Set the architectures for which to build.
set(CMAKE_OSX_ARCHITECTURES x86_64 CACHE STRING "" FORCE)
# Setting CMAKE_OSX_SYSROOT SDK, when using Xcode generator the name is enough
# but full path is necessary for others
set(CMAKE_OSX_SYSROOT macosx CACHE STRING "" FORCE)


string(APPEND CONAN_CXX_FLAGS " -m64")
string(APPEND CONAN_C_FLAGS " -m64")
string(APPEND CONAN_SHARED_LINKER_FLAGS " -m64")
string(APPEND CONAN_EXE_LINKER_FLAGS " -m64")

string(APPEND CONAN_CXX_FLAGS " -stdlib=libc++")




if(DEFINED CONAN_CXX_FLAGS)
  string(APPEND CMAKE_CXX_FLAGS_INIT " ${CONAN_CXX_FLAGS}")
endif()
if(DEFINED CONAN_C_FLAGS)
  string(APPEND CMAKE_C_FLAGS_INIT " ${CONAN_C_FLAGS}")
endif()
if(DEFINED CONAN_SHARED_LINKER_FLAGS)
  string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " ${CONAN_SHARED_LINKER_FLAGS}")
endif()
if(DEFINED CONAN_EXE_LINKER_FLAGS)
  string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " ${CONAN_EXE_LINKER_FLAGS}")
endif()

get_property( _CMAKE_IN_TRY_COMPILE GLOBAL PROPERTY IN_TRY_COMPILE )
if(_CMAKE_IN_TRY_COMPILE)
    message(STATUS "Running toolchain IN_TRY_COMPILE")
    return()
endif()

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)

# Definition of CMAKE_MODULE_PATH
# The root (which is the default builddirs) path of dependencies in the host context
list(PREPEND CMAKE_MODULE_PATH "/Users/ricardo/.conan/data/openssl/1.1.1m/_/_/package/e4fbacdee83eb1a2e68227ffe19f6e9a61b36cbc/" "/Users/ricardo/.conan/data/yaml-cpp/0.7.0/_/_/package/1aff124b71f6d938138ecd7179ff075f4304a85d/" "/Users/ricardo/.conan/data/rapidjson/cci.20211112/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/" "/Users/ricardo/.conan/data/libwebsockets/4.3.0/_/_/package/48633482216d06ba409ce622d84c996dffe0a0c8/" "/Users/ricardo/.conan/data/sqlite3/3.38.5/_/_/package/105a8d60f8f74509af4922daa14b32b1e2ace8ac/" "/Users/ricardo/.conan/data/cpr/1.7.2/_/_/package/5af2c17d895bde9918bab0a3d581923e6f371125/" "/Users/ricardo/.conan/data/zlib/1.2.12/_/_/package/e4fbacdee83eb1a2e68227ffe19f6e9a61b36cbc/" "/Users/ricardo/.conan/data/xerces-c/3.2.3/_/_/package/8cba9d6b9e459be29f20041ae99e8db33e2cf438/" "/Users/ricardo/.conan/data/re2/20220201/_/_/package/1aff124b71f6d938138ecd7179ff075f4304a85d/" "/Users/ricardo/.conan/data/libcurl/7.80.0/_/_/package/6cf5c876f79b4b7b746438b27866d09791be95e0/")
# the generators folder (where conan generates files, like this toolchain)
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# Definition of CMAKE_PREFIX_PATH, CMAKE_XXXXX_PATH
# The Conan local "generators" folder, where this toolchain is saved.
list(PREPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR} )
list(PREPEND CMAKE_LIBRARY_PATH "/Users/ricardo/.conan/data/openssl/1.1.1m/_/_/package/e4fbacdee83eb1a2e68227ffe19f6e9a61b36cbc/lib" "/Users/ricardo/.conan/data/yaml-cpp/0.7.0/_/_/package/1aff124b71f6d938138ecd7179ff075f4304a85d/lib" "/Users/ricardo/.conan/data/rapidjson/cci.20211112/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/lib" "/Users/ricardo/.conan/data/libwebsockets/4.3.0/_/_/package/48633482216d06ba409ce622d84c996dffe0a0c8/lib" "/Users/ricardo/.conan/data/sqlite3/3.38.5/_/_/package/105a8d60f8f74509af4922daa14b32b1e2ace8ac/lib" "/Users/ricardo/.conan/data/cpr/1.7.2/_/_/package/5af2c17d895bde9918bab0a3d581923e6f371125/lib" "/Users/ricardo/.conan/data/zlib/1.2.12/_/_/package/e4fbacdee83eb1a2e68227ffe19f6e9a61b36cbc/lib" "/Users/ricardo/.conan/data/xerces-c/3.2.3/_/_/package/8cba9d6b9e459be29f20041ae99e8db33e2cf438/lib" "/Users/ricardo/.conan/data/re2/20220201/_/_/package/1aff124b71f6d938138ecd7179ff075f4304a85d/lib" "/Users/ricardo/.conan/data/libcurl/7.80.0/_/_/package/6cf5c876f79b4b7b746438b27866d09791be95e0/lib")
list(PREPEND CMAKE_FRAMEWORK_PATH "/Users/ricardo/.conan/data/openssl/1.1.1m/_/_/package/e4fbacdee83eb1a2e68227ffe19f6e9a61b36cbc/Frameworks" "/Users/ricardo/.conan/data/yaml-cpp/0.7.0/_/_/package/1aff124b71f6d938138ecd7179ff075f4304a85d/Frameworks" "/Users/ricardo/.conan/data/rapidjson/cci.20211112/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/Frameworks" "/Users/ricardo/.conan/data/libwebsockets/4.3.0/_/_/package/48633482216d06ba409ce622d84c996dffe0a0c8/Frameworks" "/Users/ricardo/.conan/data/sqlite3/3.38.5/_/_/package/105a8d60f8f74509af4922daa14b32b1e2ace8ac/Frameworks" "/Users/ricardo/.conan/data/cpr/1.7.2/_/_/package/5af2c17d895bde9918bab0a3d581923e6f371125/Frameworks" "/Users/ricardo/.conan/data/zlib/1.2.12/_/_/package/e4fbacdee83eb1a2e68227ffe19f6e9a61b36cbc/Frameworks" "/Users/ricardo/.conan/data/xerces-c/3.2.3/_/_/package/8cba9d6b9e459be29f20041ae99e8db33e2cf438/Frameworks" "/Users/ricardo/.conan/data/re2/20220201/_/_/package/1aff124b71f6d938138ecd7179ff075f4304a85d/Frameworks" "/Users/ricardo/.conan/data/libcurl/7.80.0/_/_/package/6cf5c876f79b4b7b746438b27866d09791be95e0/Frameworks")
list(PREPEND CMAKE_INCLUDE_PATH "/Users/ricardo/.conan/data/openssl/1.1.1m/_/_/package/e4fbacdee83eb1a2e68227ffe19f6e9a61b36cbc/include" "/Users/ricardo/.conan/data/yaml-cpp/0.7.0/_/_/package/1aff124b71f6d938138ecd7179ff075f4304a85d/include" "/Users/ricardo/.conan/data/rapidjson/cci.20211112/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/include" "/Users/ricardo/.conan/data/libwebsockets/4.3.0/_/_/package/48633482216d06ba409ce622d84c996dffe0a0c8/include" "/Users/ricardo/.conan/data/sqlite3/3.38.5/_/_/package/105a8d60f8f74509af4922daa14b32b1e2ace8ac/include" "/Users/ricardo/.conan/data/cpr/1.7.2/_/_/package/5af2c17d895bde9918bab0a3d581923e6f371125/include" "/Users/ricardo/.conan/data/zlib/1.2.12/_/_/package/e4fbacdee83eb1a2e68227ffe19f6e9a61b36cbc/include" "/Users/ricardo/.conan/data/xerces-c/3.2.3/_/_/package/8cba9d6b9e459be29f20041ae99e8db33e2cf438/include" "/Users/ricardo/.conan/data/re2/20220201/_/_/package/1aff124b71f6d938138ecd7179ff075f4304a85d/include" "/Users/ricardo/.conan/data/libcurl/7.80.0/_/_/package/6cf5c876f79b4b7b746438b27866d09791be95e0/include")





# Variables
# Variables  per configuration


# Preprocessor definitions
# Preprocessor definitions per configuration

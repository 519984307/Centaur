

# Conan automatically generated toolchain file
# DO NOT EDIT MANUALLY, it will be overwritten

# Avoid including toolchain file several times (bad if appending to variables like
#   CMAKE_CXX_FLAGS. See https://github.com/android/ndk/issues/323
include_guard()

message(STATUS "Using Conan toolchain: ${CMAKE_CURRENT_LIST_FILE}")

if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeToolchain' generator only works with CMake >= 3.15")
endif()





set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)


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
list(PREPEND CMAKE_MODULE_PATH "/Users/ricardo/.conan/data/openssl/1.1.1m/_/_/package/f0ce0f189afed95b41fb2faad2f2225a7eca0d22/" "/Users/ricardo/.conan/data/yaml-cpp/0.7.0/_/_/package/b5855232fa6dee7d27d2005b789408c1c12da382/" "/Users/ricardo/.conan/data/rapidjson/cci.20211112/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/" "/Users/ricardo/.conan/data/libwebsockets/4.3.0/_/_/package/c7d7e09f53f1db9545a5c8ccd087c20889773a20/" "/Users/ricardo/.conan/data/sqlite3/3.38.5/_/_/package/3d2c009873ddcd5176cbc0654fbdc8d5b85096ce/" "/Users/ricardo/.conan/data/cpr/1.7.2/_/_/package/c39ab15d31fe7875fedaca9aaa72e6bcfed578f5/" "/Users/ricardo/.conan/data/zlib/1.2.12/_/_/package/f0ce0f189afed95b41fb2faad2f2225a7eca0d22/" "/Users/ricardo/.conan/data/xerces-c/3.2.3/_/_/package/115d6fd4fe9a9c4a3af7803463aa17617196e87c/" "/Users/ricardo/.conan/data/re2/20220201/_/_/package/b5855232fa6dee7d27d2005b789408c1c12da382/" "/Users/ricardo/.conan/data/libcurl/7.80.0/_/_/package/63a1fd6bb0025ff45e6d194ce99323758991cb09/")
# the generators folder (where conan generates files, like this toolchain)
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# Definition of CMAKE_PREFIX_PATH, CMAKE_XXXXX_PATH
# The Conan local "generators" folder, where this toolchain is saved.
list(PREPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR} )
list(PREPEND CMAKE_LIBRARY_PATH "/Users/ricardo/.conan/data/openssl/1.1.1m/_/_/package/f0ce0f189afed95b41fb2faad2f2225a7eca0d22/lib" "/Users/ricardo/.conan/data/yaml-cpp/0.7.0/_/_/package/b5855232fa6dee7d27d2005b789408c1c12da382/lib" "/Users/ricardo/.conan/data/rapidjson/cci.20211112/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/lib" "/Users/ricardo/.conan/data/libwebsockets/4.3.0/_/_/package/c7d7e09f53f1db9545a5c8ccd087c20889773a20/lib" "/Users/ricardo/.conan/data/sqlite3/3.38.5/_/_/package/3d2c009873ddcd5176cbc0654fbdc8d5b85096ce/lib" "/Users/ricardo/.conan/data/cpr/1.7.2/_/_/package/c39ab15d31fe7875fedaca9aaa72e6bcfed578f5/lib" "/Users/ricardo/.conan/data/zlib/1.2.12/_/_/package/f0ce0f189afed95b41fb2faad2f2225a7eca0d22/lib" "/Users/ricardo/.conan/data/xerces-c/3.2.3/_/_/package/115d6fd4fe9a9c4a3af7803463aa17617196e87c/lib" "/Users/ricardo/.conan/data/re2/20220201/_/_/package/b5855232fa6dee7d27d2005b789408c1c12da382/lib" "/Users/ricardo/.conan/data/libcurl/7.80.0/_/_/package/63a1fd6bb0025ff45e6d194ce99323758991cb09/lib")
list(PREPEND CMAKE_FRAMEWORK_PATH "/Users/ricardo/.conan/data/openssl/1.1.1m/_/_/package/f0ce0f189afed95b41fb2faad2f2225a7eca0d22/Frameworks" "/Users/ricardo/.conan/data/yaml-cpp/0.7.0/_/_/package/b5855232fa6dee7d27d2005b789408c1c12da382/Frameworks" "/Users/ricardo/.conan/data/rapidjson/cci.20211112/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/Frameworks" "/Users/ricardo/.conan/data/libwebsockets/4.3.0/_/_/package/c7d7e09f53f1db9545a5c8ccd087c20889773a20/Frameworks" "/Users/ricardo/.conan/data/sqlite3/3.38.5/_/_/package/3d2c009873ddcd5176cbc0654fbdc8d5b85096ce/Frameworks" "/Users/ricardo/.conan/data/cpr/1.7.2/_/_/package/c39ab15d31fe7875fedaca9aaa72e6bcfed578f5/Frameworks" "/Users/ricardo/.conan/data/zlib/1.2.12/_/_/package/f0ce0f189afed95b41fb2faad2f2225a7eca0d22/Frameworks" "/Users/ricardo/.conan/data/xerces-c/3.2.3/_/_/package/115d6fd4fe9a9c4a3af7803463aa17617196e87c/Frameworks" "/Users/ricardo/.conan/data/re2/20220201/_/_/package/b5855232fa6dee7d27d2005b789408c1c12da382/Frameworks" "/Users/ricardo/.conan/data/libcurl/7.80.0/_/_/package/63a1fd6bb0025ff45e6d194ce99323758991cb09/Frameworks")
list(PREPEND CMAKE_INCLUDE_PATH "/Users/ricardo/.conan/data/openssl/1.1.1m/_/_/package/f0ce0f189afed95b41fb2faad2f2225a7eca0d22/include" "/Users/ricardo/.conan/data/yaml-cpp/0.7.0/_/_/package/b5855232fa6dee7d27d2005b789408c1c12da382/include" "/Users/ricardo/.conan/data/rapidjson/cci.20211112/_/_/package/5ab84d6acfe1f23c4fae0ab88f26e3a396351ac9/include" "/Users/ricardo/.conan/data/libwebsockets/4.3.0/_/_/package/c7d7e09f53f1db9545a5c8ccd087c20889773a20/include" "/Users/ricardo/.conan/data/sqlite3/3.38.5/_/_/package/3d2c009873ddcd5176cbc0654fbdc8d5b85096ce/include" "/Users/ricardo/.conan/data/cpr/1.7.2/_/_/package/c39ab15d31fe7875fedaca9aaa72e6bcfed578f5/include" "/Users/ricardo/.conan/data/zlib/1.2.12/_/_/package/f0ce0f189afed95b41fb2faad2f2225a7eca0d22/include" "/Users/ricardo/.conan/data/xerces-c/3.2.3/_/_/package/115d6fd4fe9a9c4a3af7803463aa17617196e87c/include" "/Users/ricardo/.conan/data/re2/20220201/_/_/package/b5855232fa6dee7d27d2005b789408c1c12da382/include" "/Users/ricardo/.conan/data/libcurl/7.80.0/_/_/package/63a1fd6bb0025ff45e6d194ce99323758991cb09/include")





# Variables
# Variables  per configuration


# Preprocessor definitions
# Preprocessor definitions per configuration

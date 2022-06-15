########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

include(FindPackageHandleStandardArgs)
set(CURL_FOUND 1)
set(CURL_VERSION "7.80.0")

find_package_handle_standard_args(CURL
                                  REQUIRED_VARS CURL_VERSION
                                  VERSION_VAR CURL_VERSION)
mark_as_advanced(CURL_FOUND CURL_VERSION)

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/module-CURLTargets.cmake)
include(CMakeFindDependencyMacro)

foreach(_DEPENDENCY ${libcurl_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(CURL_VERSION_STRING "7.80.0")
set(CURL_INCLUDE_DIRS ${libcurl_INCLUDE_DIRS_DEBUG} )
set(CURL_INCLUDE_DIR ${libcurl_INCLUDE_DIRS_DEBUG} )
set(CURL_LIBRARIES ${libcurl_LIBRARIES_DEBUG} )
set(CURL_DEFINITIONS ${libcurl_DEFINITIONS_DEBUG} )

# Only the first installed configuration is included to avoid the collision
foreach(_BUILD_MODULE ${libcurl_BUILD_MODULES_PATHS_DEBUG} )
    conan_message(STATUS "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


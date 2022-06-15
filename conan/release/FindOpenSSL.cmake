########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

include(FindPackageHandleStandardArgs)
set(OpenSSL_FOUND 1)
set(OpenSSL_VERSION "1.1.1m")

find_package_handle_standard_args(OpenSSL
                                  REQUIRED_VARS OpenSSL_VERSION
                                  VERSION_VAR OpenSSL_VERSION)
mark_as_advanced(OpenSSL_FOUND OpenSSL_VERSION)

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/module-OpenSSLTargets.cmake)
include(CMakeFindDependencyMacro)

foreach(_DEPENDENCY ${openssl_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(OpenSSL_VERSION_STRING "1.1.1m")
set(OpenSSL_INCLUDE_DIRS ${openssl_INCLUDE_DIRS_RELEASE} )
set(OpenSSL_INCLUDE_DIR ${openssl_INCLUDE_DIRS_RELEASE} )
set(OpenSSL_LIBRARIES ${openssl_LIBRARIES_RELEASE} )
set(OpenSSL_DEFINITIONS ${openssl_DEFINITIONS_RELEASE} )

# Only the first installed configuration is included to avoid the collision
foreach(_BUILD_MODULE ${openssl_BUILD_MODULES_PATHS_RELEASE} )
    conan_message(STATUS "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


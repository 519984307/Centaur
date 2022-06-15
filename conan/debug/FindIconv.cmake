########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

include(FindPackageHandleStandardArgs)
set(Iconv_FOUND 1)
set(Iconv_VERSION "1.16")

find_package_handle_standard_args(Iconv
                                  REQUIRED_VARS Iconv_VERSION
                                  VERSION_VAR Iconv_VERSION)
mark_as_advanced(Iconv_FOUND Iconv_VERSION)

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/module-IconvTargets.cmake)
include(CMakeFindDependencyMacro)

foreach(_DEPENDENCY ${libiconv_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(Iconv_VERSION_STRING "1.16")
set(Iconv_INCLUDE_DIRS ${libiconv_INCLUDE_DIRS_DEBUG} )
set(Iconv_INCLUDE_DIR ${libiconv_INCLUDE_DIRS_DEBUG} )
set(Iconv_LIBRARIES ${libiconv_LIBRARIES_DEBUG} )
set(Iconv_DEFINITIONS ${libiconv_DEFINITIONS_DEBUG} )

# Only the first installed configuration is included to avoid the collision
foreach(_BUILD_MODULE ${libiconv_BUILD_MODULES_PATHS_DEBUG} )
    conan_message(STATUS "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


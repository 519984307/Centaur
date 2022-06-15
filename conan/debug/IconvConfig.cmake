########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()


include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/IconvTargets.cmake)
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


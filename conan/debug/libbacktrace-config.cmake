########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()


include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/libbacktraceTargets.cmake)
include(CMakeFindDependencyMacro)

foreach(_DEPENDENCY ${libbacktrace_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(libbacktrace_VERSION_STRING "cci.20210118")
set(libbacktrace_INCLUDE_DIRS ${libbacktrace_INCLUDE_DIRS_DEBUG} )
set(libbacktrace_INCLUDE_DIR ${libbacktrace_INCLUDE_DIRS_DEBUG} )
set(libbacktrace_LIBRARIES ${libbacktrace_LIBRARIES_DEBUG} )
set(libbacktrace_DEFINITIONS ${libbacktrace_DEFINITIONS_DEBUG} )

# Only the first installed configuration is included to avoid the collision
foreach(_BUILD_MODULE ${libbacktrace_BUILD_MODULES_PATHS_DEBUG} )
    conan_message(STATUS "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


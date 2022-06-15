########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

include(FindPackageHandleStandardArgs)
set(XercesC_FOUND 1)
set(XercesC_VERSION "3.2.3")

find_package_handle_standard_args(XercesC
                                  REQUIRED_VARS XercesC_VERSION
                                  VERSION_VAR XercesC_VERSION)
mark_as_advanced(XercesC_FOUND XercesC_VERSION)

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/module-XercesCTargets.cmake)
include(CMakeFindDependencyMacro)

foreach(_DEPENDENCY ${xerces-c_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(XercesC_VERSION_STRING "3.2.3")
set(XercesC_INCLUDE_DIRS ${xerces-c_INCLUDE_DIRS_DEBUG} )
set(XercesC_INCLUDE_DIR ${xerces-c_INCLUDE_DIRS_DEBUG} )
set(XercesC_LIBRARIES ${xerces-c_LIBRARIES_DEBUG} )
set(XercesC_DEFINITIONS ${xerces-c_DEFINITIONS_DEBUG} )

# Only the first installed configuration is included to avoid the collision
foreach(_BUILD_MODULE ${xerces-c_BUILD_MODULES_PATHS_DEBUG} )
    conan_message(STATUS "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


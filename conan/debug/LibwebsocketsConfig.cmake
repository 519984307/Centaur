########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()


include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/LibwebsocketsTargets.cmake)
include(CMakeFindDependencyMacro)

foreach(_DEPENDENCY ${libwebsockets_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(Libwebsockets_VERSION_STRING "4.3.0")
set(Libwebsockets_INCLUDE_DIRS ${libwebsockets_INCLUDE_DIRS_DEBUG} )
set(Libwebsockets_INCLUDE_DIR ${libwebsockets_INCLUDE_DIRS_DEBUG} )
set(Libwebsockets_LIBRARIES ${libwebsockets_LIBRARIES_DEBUG} )
set(Libwebsockets_DEFINITIONS ${libwebsockets_DEFINITIONS_DEBUG} )

# Only the first installed configuration is included to avoid the collision
foreach(_BUILD_MODULE ${libwebsockets_BUILD_MODULES_PATHS_DEBUG} )
    conan_message(STATUS "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()

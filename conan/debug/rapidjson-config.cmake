########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()


include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/rapidjsonTargets.cmake)
include(CMakeFindDependencyMacro)

foreach(_DEPENDENCY ${rapidjson_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(rapidjson_VERSION_STRING "cci.20211112")
set(rapidjson_INCLUDE_DIRS ${rapidjson_INCLUDE_DIRS_DEBUG} )
set(rapidjson_INCLUDE_DIR ${rapidjson_INCLUDE_DIRS_DEBUG} )
set(rapidjson_LIBRARIES ${rapidjson_LIBRARIES_DEBUG} )
set(rapidjson_DEFINITIONS ${rapidjson_DEFINITIONS_DEBUG} )

# Only the first installed configuration is included to avoid the collision
foreach(_BUILD_MODULE ${rapidjson_BUILD_MODULES_PATHS_DEBUG} )
    conan_message(STATUS "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


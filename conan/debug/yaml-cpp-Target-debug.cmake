########### VARIABLES #######################################################################
#############################################################################################

set(yaml-cpp_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${yaml-cpp_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${yaml-cpp_COMPILE_OPTIONS_C_DEBUG}>")

set(yaml-cpp_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${yaml-cpp_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${yaml-cpp_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${yaml-cpp_EXE_LINK_FLAGS_DEBUG}>")

set(yaml-cpp_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(yaml-cpp_FRAMEWORKS_FOUND_DEBUG "${yaml-cpp_FRAMEWORKS_DEBUG}" "${yaml-cpp_FRAMEWORK_DIRS_DEBUG}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_yaml-cpp_DEPENDENCIES_DEBUG "${yaml-cpp_FRAMEWORKS_FOUND_DEBUG} ${yaml-cpp_SYSTEM_LIBS_DEBUG} ")

set(yaml-cpp_LIBRARIES_TARGETS_DEBUG "") # Will be filled later
set(yaml-cpp_LIBRARIES_DEBUG "") # Will be filled later
conan_package_library_targets("${yaml-cpp_LIBS_DEBUG}"    # libraries
                              "${yaml-cpp_LIB_DIRS_DEBUG}" # package_libdir
                              "${_yaml-cpp_DEPENDENCIES_DEBUG}" # deps
                              yaml-cpp_LIBRARIES_DEBUG   # out_libraries
                              yaml-cpp_LIBRARIES_TARGETS_DEBUG  # out_libraries_targets
                              "_DEBUG"  # config_suffix
                              "yaml-cpp")    # package_name

foreach(_FRAMEWORK ${yaml-cpp_FRAMEWORKS_FOUND_DEBUG})
    list(APPEND yaml-cpp_LIBRARIES_TARGETS_DEBUG ${_FRAMEWORK})
    list(APPEND yaml-cpp_LIBRARIES_DEBUG ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${yaml-cpp_SYSTEM_LIBS_DEBUG})
    list(APPEND yaml-cpp_LIBRARIES_TARGETS_DEBUG ${_SYSTEM_LIB})
    list(APPEND yaml-cpp_LIBRARIES_DEBUG ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(yaml-cpp_LIBRARIES_TARGETS_DEBUG "${yaml-cpp_LIBRARIES_TARGETS_DEBUG};")
set(yaml-cpp_LIBRARIES_DEBUG "${yaml-cpp_LIBRARIES_DEBUG};")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${yaml-cpp_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${yaml-cpp_BUILD_DIRS_DEBUG} ${CMAKE_PREFIX_PATH})


########## GLOBAL TARGET PROPERTIES Debug ########################################
set_property(TARGET yaml-cpp
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${yaml-cpp_LIBRARIES_TARGETS_DEBUG}
                                           ${yaml-cpp_OBJECTS_DEBUG}> APPEND)
set_property(TARGET yaml-cpp
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${yaml-cpp_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET yaml-cpp
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${yaml-cpp_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET yaml-cpp
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${yaml-cpp_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET yaml-cpp
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:${yaml-cpp_COMPILE_OPTIONS_DEBUG}> APPEND)

########## COMPONENTS TARGET PROPERTIES Debug ########################################
########### VARIABLES #######################################################################
#############################################################################################

set(re2_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${re2_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${re2_COMPILE_OPTIONS_C_DEBUG}>")

set(re2_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${re2_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${re2_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${re2_EXE_LINK_FLAGS_DEBUG}>")

set(re2_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(re2_FRAMEWORKS_FOUND_DEBUG "${re2_FRAMEWORKS_DEBUG}" "${re2_FRAMEWORK_DIRS_DEBUG}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_re2_DEPENDENCIES_DEBUG "${re2_FRAMEWORKS_FOUND_DEBUG} ${re2_SYSTEM_LIBS_DEBUG} ")

set(re2_LIBRARIES_TARGETS_DEBUG "") # Will be filled later
set(re2_LIBRARIES_DEBUG "") # Will be filled later
conan_package_library_targets("${re2_LIBS_DEBUG}"    # libraries
                              "${re2_LIB_DIRS_DEBUG}" # package_libdir
                              "${_re2_DEPENDENCIES_DEBUG}" # deps
                              re2_LIBRARIES_DEBUG   # out_libraries
                              re2_LIBRARIES_TARGETS_DEBUG  # out_libraries_targets
                              "_DEBUG"  # config_suffix
                              "re2")    # package_name

foreach(_FRAMEWORK ${re2_FRAMEWORKS_FOUND_DEBUG})
    list(APPEND re2_LIBRARIES_TARGETS_DEBUG ${_FRAMEWORK})
    list(APPEND re2_LIBRARIES_DEBUG ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${re2_SYSTEM_LIBS_DEBUG})
    list(APPEND re2_LIBRARIES_TARGETS_DEBUG ${_SYSTEM_LIB})
    list(APPEND re2_LIBRARIES_DEBUG ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(re2_LIBRARIES_TARGETS_DEBUG "${re2_LIBRARIES_TARGETS_DEBUG};")
set(re2_LIBRARIES_DEBUG "${re2_LIBRARIES_DEBUG};")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${re2_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${re2_BUILD_DIRS_DEBUG} ${CMAKE_PREFIX_PATH})


########## GLOBAL TARGET PROPERTIES Debug ########################################
set_property(TARGET re2::re2
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${re2_LIBRARIES_TARGETS_DEBUG}
                                           ${re2_OBJECTS_DEBUG}> APPEND)
set_property(TARGET re2::re2
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${re2_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET re2::re2
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${re2_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET re2::re2
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${re2_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET re2::re2
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:${re2_COMPILE_OPTIONS_DEBUG}> APPEND)

########## COMPONENTS TARGET PROPERTIES Debug ########################################
########### VARIABLES #######################################################################
#############################################################################################

set(libbacktrace_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${libbacktrace_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${libbacktrace_COMPILE_OPTIONS_C_DEBUG}>")

set(libbacktrace_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libbacktrace_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libbacktrace_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libbacktrace_EXE_LINK_FLAGS_DEBUG}>")

set(libbacktrace_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(libbacktrace_FRAMEWORKS_FOUND_DEBUG "${libbacktrace_FRAMEWORKS_DEBUG}" "${libbacktrace_FRAMEWORK_DIRS_DEBUG}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_libbacktrace_DEPENDENCIES_DEBUG "${libbacktrace_FRAMEWORKS_FOUND_DEBUG} ${libbacktrace_SYSTEM_LIBS_DEBUG} ")

set(libbacktrace_LIBRARIES_TARGETS_DEBUG "") # Will be filled later
set(libbacktrace_LIBRARIES_DEBUG "") # Will be filled later
conan_package_library_targets("${libbacktrace_LIBS_DEBUG}"    # libraries
                              "${libbacktrace_LIB_DIRS_DEBUG}" # package_libdir
                              "${_libbacktrace_DEPENDENCIES_DEBUG}" # deps
                              libbacktrace_LIBRARIES_DEBUG   # out_libraries
                              libbacktrace_LIBRARIES_TARGETS_DEBUG  # out_libraries_targets
                              "_DEBUG"  # config_suffix
                              "libbacktrace")    # package_name

foreach(_FRAMEWORK ${libbacktrace_FRAMEWORKS_FOUND_DEBUG})
    list(APPEND libbacktrace_LIBRARIES_TARGETS_DEBUG ${_FRAMEWORK})
    list(APPEND libbacktrace_LIBRARIES_DEBUG ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${libbacktrace_SYSTEM_LIBS_DEBUG})
    list(APPEND libbacktrace_LIBRARIES_TARGETS_DEBUG ${_SYSTEM_LIB})
    list(APPEND libbacktrace_LIBRARIES_DEBUG ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(libbacktrace_LIBRARIES_TARGETS_DEBUG "${libbacktrace_LIBRARIES_TARGETS_DEBUG};")
set(libbacktrace_LIBRARIES_DEBUG "${libbacktrace_LIBRARIES_DEBUG};")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${libbacktrace_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${libbacktrace_BUILD_DIRS_DEBUG} ${CMAKE_PREFIX_PATH})


########## GLOBAL TARGET PROPERTIES Debug ########################################
set_property(TARGET libbacktrace::libbacktrace
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${libbacktrace_LIBRARIES_TARGETS_DEBUG}
                                           ${libbacktrace_OBJECTS_DEBUG}> APPEND)
set_property(TARGET libbacktrace::libbacktrace
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${libbacktrace_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET libbacktrace::libbacktrace
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${libbacktrace_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET libbacktrace::libbacktrace
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${libbacktrace_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET libbacktrace::libbacktrace
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:${libbacktrace_COMPILE_OPTIONS_DEBUG}> APPEND)

########## COMPONENTS TARGET PROPERTIES Debug ########################################
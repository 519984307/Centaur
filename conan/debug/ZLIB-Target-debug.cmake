########### VARIABLES #######################################################################
#############################################################################################

set(zlib_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${zlib_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${zlib_COMPILE_OPTIONS_C_DEBUG}>")

set(zlib_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${zlib_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${zlib_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${zlib_EXE_LINK_FLAGS_DEBUG}>")

set(zlib_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(zlib_FRAMEWORKS_FOUND_DEBUG "${zlib_FRAMEWORKS_DEBUG}" "${zlib_FRAMEWORK_DIRS_DEBUG}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_zlib_DEPENDENCIES_DEBUG "${zlib_FRAMEWORKS_FOUND_DEBUG} ${zlib_SYSTEM_LIBS_DEBUG} ")

set(zlib_LIBRARIES_TARGETS_DEBUG "") # Will be filled later
set(zlib_LIBRARIES_DEBUG "") # Will be filled later
conan_package_library_targets("${zlib_LIBS_DEBUG}"    # libraries
                              "${zlib_LIB_DIRS_DEBUG}" # package_libdir
                              "${_zlib_DEPENDENCIES_DEBUG}" # deps
                              zlib_LIBRARIES_DEBUG   # out_libraries
                              zlib_LIBRARIES_TARGETS_DEBUG  # out_libraries_targets
                              "_DEBUG"  # config_suffix
                              "zlib")    # package_name

foreach(_FRAMEWORK ${zlib_FRAMEWORKS_FOUND_DEBUG})
    list(APPEND zlib_LIBRARIES_TARGETS_DEBUG ${_FRAMEWORK})
    list(APPEND zlib_LIBRARIES_DEBUG ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${zlib_SYSTEM_LIBS_DEBUG})
    list(APPEND zlib_LIBRARIES_TARGETS_DEBUG ${_SYSTEM_LIB})
    list(APPEND zlib_LIBRARIES_DEBUG ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(zlib_LIBRARIES_TARGETS_DEBUG "${zlib_LIBRARIES_TARGETS_DEBUG};")
set(zlib_LIBRARIES_DEBUG "${zlib_LIBRARIES_DEBUG};")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${zlib_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${zlib_BUILD_DIRS_DEBUG} ${CMAKE_PREFIX_PATH})


########## GLOBAL TARGET PROPERTIES Debug ########################################
set_property(TARGET ZLIB::ZLIB
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${zlib_LIBRARIES_TARGETS_DEBUG}
                                           ${zlib_OBJECTS_DEBUG}> APPEND)
set_property(TARGET ZLIB::ZLIB
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${zlib_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET ZLIB::ZLIB
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${zlib_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET ZLIB::ZLIB
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${zlib_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET ZLIB::ZLIB
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:${zlib_COMPILE_OPTIONS_DEBUG}> APPEND)

########## COMPONENTS TARGET PROPERTIES Debug ########################################
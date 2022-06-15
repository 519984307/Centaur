########### VARIABLES #######################################################################
#############################################################################################

set(bzip2_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${bzip2_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${bzip2_COMPILE_OPTIONS_C_DEBUG}>")

set(bzip2_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${bzip2_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${bzip2_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${bzip2_EXE_LINK_FLAGS_DEBUG}>")

set(bzip2_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(bzip2_FRAMEWORKS_FOUND_DEBUG "${bzip2_FRAMEWORKS_DEBUG}" "${bzip2_FRAMEWORK_DIRS_DEBUG}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_bzip2_DEPENDENCIES_DEBUG "${bzip2_FRAMEWORKS_FOUND_DEBUG} ${bzip2_SYSTEM_LIBS_DEBUG} ")

set(bzip2_LIBRARIES_TARGETS_DEBUG "") # Will be filled later
set(bzip2_LIBRARIES_DEBUG "") # Will be filled later
conan_package_library_targets("${bzip2_LIBS_DEBUG}"    # libraries
                              "${bzip2_LIB_DIRS_DEBUG}" # package_libdir
                              "${_bzip2_DEPENDENCIES_DEBUG}" # deps
                              bzip2_LIBRARIES_DEBUG   # out_libraries
                              bzip2_LIBRARIES_TARGETS_DEBUG  # out_libraries_targets
                              "_DEBUG"  # config_suffix
                              "bzip2")    # package_name

foreach(_FRAMEWORK ${bzip2_FRAMEWORKS_FOUND_DEBUG})
    list(APPEND bzip2_LIBRARIES_TARGETS_DEBUG ${_FRAMEWORK})
    list(APPEND bzip2_LIBRARIES_DEBUG ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${bzip2_SYSTEM_LIBS_DEBUG})
    list(APPEND bzip2_LIBRARIES_TARGETS_DEBUG ${_SYSTEM_LIB})
    list(APPEND bzip2_LIBRARIES_DEBUG ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(bzip2_LIBRARIES_TARGETS_DEBUG "${bzip2_LIBRARIES_TARGETS_DEBUG};")
set(bzip2_LIBRARIES_DEBUG "${bzip2_LIBRARIES_DEBUG};")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${bzip2_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${bzip2_BUILD_DIRS_DEBUG} ${CMAKE_PREFIX_PATH})


########## GLOBAL TARGET PROPERTIES Debug ########################################
set_property(TARGET BZip2::BZip2
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${bzip2_LIBRARIES_TARGETS_DEBUG}
                                           ${bzip2_OBJECTS_DEBUG}> APPEND)
set_property(TARGET BZip2::BZip2
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${bzip2_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET BZip2::BZip2
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${bzip2_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET BZip2::BZip2
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${bzip2_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET BZip2::BZip2
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:${bzip2_COMPILE_OPTIONS_DEBUG}> APPEND)

########## COMPONENTS TARGET PROPERTIES Debug ########################################
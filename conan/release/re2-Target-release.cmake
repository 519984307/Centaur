########### VARIABLES #######################################################################
#############################################################################################

set(re2_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${re2_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${re2_COMPILE_OPTIONS_C_RELEASE}>")

set(re2_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${re2_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${re2_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${re2_EXE_LINK_FLAGS_RELEASE}>")

set(re2_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(re2_FRAMEWORKS_FOUND_RELEASE "${re2_FRAMEWORKS_RELEASE}" "${re2_FRAMEWORK_DIRS_RELEASE}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_re2_DEPENDENCIES_RELEASE "${re2_FRAMEWORKS_FOUND_RELEASE} ${re2_SYSTEM_LIBS_RELEASE} ")

set(re2_LIBRARIES_TARGETS_RELEASE "") # Will be filled later
set(re2_LIBRARIES_RELEASE "") # Will be filled later
conan_package_library_targets("${re2_LIBS_RELEASE}"    # libraries
                              "${re2_LIB_DIRS_RELEASE}" # package_libdir
                              "${_re2_DEPENDENCIES_RELEASE}" # deps
                              re2_LIBRARIES_RELEASE   # out_libraries
                              re2_LIBRARIES_TARGETS_RELEASE  # out_libraries_targets
                              "_RELEASE"  # config_suffix
                              "re2")    # package_name

foreach(_FRAMEWORK ${re2_FRAMEWORKS_FOUND_RELEASE})
    list(APPEND re2_LIBRARIES_TARGETS_RELEASE ${_FRAMEWORK})
    list(APPEND re2_LIBRARIES_RELEASE ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${re2_SYSTEM_LIBS_RELEASE})
    list(APPEND re2_LIBRARIES_TARGETS_RELEASE ${_SYSTEM_LIB})
    list(APPEND re2_LIBRARIES_RELEASE ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(re2_LIBRARIES_TARGETS_RELEASE "${re2_LIBRARIES_TARGETS_RELEASE};")
set(re2_LIBRARIES_RELEASE "${re2_LIBRARIES_RELEASE};")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${re2_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${re2_BUILD_DIRS_RELEASE} ${CMAKE_PREFIX_PATH})


########## GLOBAL TARGET PROPERTIES Release ########################################
set_property(TARGET re2::re2
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${re2_LIBRARIES_TARGETS_RELEASE}
                                           ${re2_OBJECTS_RELEASE}> APPEND)
set_property(TARGET re2::re2
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Release>:${re2_LINKER_FLAGS_RELEASE}> APPEND)
set_property(TARGET re2::re2
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Release>:${re2_INCLUDE_DIRS_RELEASE}> APPEND)
set_property(TARGET re2::re2
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Release>:${re2_COMPILE_DEFINITIONS_RELEASE}> APPEND)
set_property(TARGET re2::re2
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Release>:${re2_COMPILE_OPTIONS_RELEASE}> APPEND)

########## COMPONENTS TARGET PROPERTIES Release ########################################
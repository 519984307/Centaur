########### VARIABLES #######################################################################
#############################################################################################

set(zlib_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${zlib_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${zlib_COMPILE_OPTIONS_C_RELEASE}>")

set(zlib_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${zlib_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${zlib_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${zlib_EXE_LINK_FLAGS_RELEASE}>")

set(zlib_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(zlib_FRAMEWORKS_FOUND_RELEASE "${zlib_FRAMEWORKS_RELEASE}" "${zlib_FRAMEWORK_DIRS_RELEASE}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_zlib_DEPENDENCIES_RELEASE "${zlib_FRAMEWORKS_FOUND_RELEASE} ${zlib_SYSTEM_LIBS_RELEASE} ")

set(zlib_LIBRARIES_TARGETS_RELEASE "") # Will be filled later
set(zlib_LIBRARIES_RELEASE "") # Will be filled later
conan_package_library_targets("${zlib_LIBS_RELEASE}"    # libraries
                              "${zlib_LIB_DIRS_RELEASE}" # package_libdir
                              "${_zlib_DEPENDENCIES_RELEASE}" # deps
                              zlib_LIBRARIES_RELEASE   # out_libraries
                              zlib_LIBRARIES_TARGETS_RELEASE  # out_libraries_targets
                              "_RELEASE"  # config_suffix
                              "zlib")    # package_name

foreach(_FRAMEWORK ${zlib_FRAMEWORKS_FOUND_RELEASE})
    list(APPEND zlib_LIBRARIES_TARGETS_RELEASE ${_FRAMEWORK})
    list(APPEND zlib_LIBRARIES_RELEASE ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${zlib_SYSTEM_LIBS_RELEASE})
    list(APPEND zlib_LIBRARIES_TARGETS_RELEASE ${_SYSTEM_LIB})
    list(APPEND zlib_LIBRARIES_RELEASE ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(zlib_LIBRARIES_TARGETS_RELEASE "${zlib_LIBRARIES_TARGETS_RELEASE};")
set(zlib_LIBRARIES_RELEASE "${zlib_LIBRARIES_RELEASE};")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${zlib_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${zlib_BUILD_DIRS_RELEASE} ${CMAKE_PREFIX_PATH})


########## GLOBAL TARGET PROPERTIES Release ########################################
set_property(TARGET ZLIB::ZLIB
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${zlib_LIBRARIES_TARGETS_RELEASE}
                                           ${zlib_OBJECTS_RELEASE}> APPEND)
set_property(TARGET ZLIB::ZLIB
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Release>:${zlib_LINKER_FLAGS_RELEASE}> APPEND)
set_property(TARGET ZLIB::ZLIB
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Release>:${zlib_INCLUDE_DIRS_RELEASE}> APPEND)
set_property(TARGET ZLIB::ZLIB
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Release>:${zlib_COMPILE_DEFINITIONS_RELEASE}> APPEND)
set_property(TARGET ZLIB::ZLIB
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Release>:${zlib_COMPILE_OPTIONS_RELEASE}> APPEND)

########## COMPONENTS TARGET PROPERTIES Release ########################################
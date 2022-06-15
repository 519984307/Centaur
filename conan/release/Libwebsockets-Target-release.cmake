########### VARIABLES #######################################################################
#############################################################################################

set(libwebsockets_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${libwebsockets_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${libwebsockets_COMPILE_OPTIONS_C_RELEASE}>")

set(libwebsockets_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libwebsockets_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libwebsockets_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libwebsockets_EXE_LINK_FLAGS_RELEASE}>")

set(libwebsockets_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(libwebsockets_FRAMEWORKS_FOUND_RELEASE "${libwebsockets_FRAMEWORKS_RELEASE}" "${libwebsockets_FRAMEWORK_DIRS_RELEASE}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_libwebsockets_DEPENDENCIES_RELEASE "${libwebsockets_FRAMEWORKS_FOUND_RELEASE} ${libwebsockets_SYSTEM_LIBS_RELEASE} openssl::openssl")

set(libwebsockets_LIBRARIES_TARGETS_RELEASE "") # Will be filled later
set(libwebsockets_LIBRARIES_RELEASE "") # Will be filled later
conan_package_library_targets("${libwebsockets_LIBS_RELEASE}"    # libraries
                              "${libwebsockets_LIB_DIRS_RELEASE}" # package_libdir
                              "${_libwebsockets_DEPENDENCIES_RELEASE}" # deps
                              libwebsockets_LIBRARIES_RELEASE   # out_libraries
                              libwebsockets_LIBRARIES_TARGETS_RELEASE  # out_libraries_targets
                              "_RELEASE"  # config_suffix
                              "libwebsockets")    # package_name

foreach(_FRAMEWORK ${libwebsockets_FRAMEWORKS_FOUND_RELEASE})
    list(APPEND libwebsockets_LIBRARIES_TARGETS_RELEASE ${_FRAMEWORK})
    list(APPEND libwebsockets_LIBRARIES_RELEASE ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${libwebsockets_SYSTEM_LIBS_RELEASE})
    list(APPEND libwebsockets_LIBRARIES_TARGETS_RELEASE ${_SYSTEM_LIB})
    list(APPEND libwebsockets_LIBRARIES_RELEASE ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(libwebsockets_LIBRARIES_TARGETS_RELEASE "${libwebsockets_LIBRARIES_TARGETS_RELEASE};openssl::openssl")
set(libwebsockets_LIBRARIES_RELEASE "${libwebsockets_LIBRARIES_RELEASE};openssl::openssl")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${libwebsockets_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${libwebsockets_BUILD_DIRS_RELEASE} ${CMAKE_PREFIX_PATH})
########## COMPONENT websockets FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(libwebsockets_websockets_FRAMEWORKS_FOUND_RELEASE "")
conan_find_apple_frameworks(libwebsockets_websockets_FRAMEWORKS_FOUND_RELEASE "${libwebsockets_websockets_FRAMEWORKS_RELEASE}" "${libwebsockets_websockets_FRAMEWORK_DIRS_RELEASE}")

set(libwebsockets_websockets_LIB_TARGETS_RELEASE "")
set(libwebsockets_websockets_NOT_USED_RELEASE "")
set(libwebsockets_websockets_LIBS_FRAMEWORKS_DEPS_RELEASE ${libwebsockets_websockets_FRAMEWORKS_FOUND_RELEASE} ${libwebsockets_websockets_SYSTEM_LIBS_RELEASE} ${libwebsockets_websockets_DEPENDENCIES_RELEASE})
conan_package_library_targets("${libwebsockets_websockets_LIBS_RELEASE}"
                              "${libwebsockets_websockets_LIB_DIRS_RELEASE}"
                              "${libwebsockets_websockets_LIBS_FRAMEWORKS_DEPS_RELEASE}"
                              libwebsockets_websockets_NOT_USED_RELEASE
                              libwebsockets_websockets_LIB_TARGETS_RELEASE
                              "_RELEASE"
                              "libwebsockets_websockets")

set(libwebsockets_websockets_LINK_LIBS_RELEASE ${libwebsockets_websockets_LIB_TARGETS_RELEASE} ${libwebsockets_websockets_LIBS_FRAMEWORKS_DEPS_RELEASE})


########## GLOBAL TARGET PROPERTIES Release ########################################
set_property(TARGET websockets
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${libwebsockets_LIBRARIES_TARGETS_RELEASE}
                                           ${libwebsockets_OBJECTS_RELEASE}> APPEND)
set_property(TARGET websockets
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Release>:${libwebsockets_LINKER_FLAGS_RELEASE}> APPEND)
set_property(TARGET websockets
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Release>:${libwebsockets_INCLUDE_DIRS_RELEASE}> APPEND)
set_property(TARGET websockets
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Release>:${libwebsockets_COMPILE_DEFINITIONS_RELEASE}> APPEND)
set_property(TARGET websockets
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Release>:${libwebsockets_COMPILE_OPTIONS_RELEASE}> APPEND)

########## COMPONENTS TARGET PROPERTIES Release ########################################
########## COMPONENT websockets TARGET PROPERTIES ######################################
set_property(TARGET websockets PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${libwebsockets_websockets_LINK_LIBS_RELEASE}
             ${libwebsockets_websockets_OBJECTS_RELEASE}> APPEND)
set_property(TARGET websockets PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Release>:${libwebsockets_websockets_LINKER_FLAGS_RELEASE}> APPEND)
set_property(TARGET websockets PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Release>:${libwebsockets_websockets_INCLUDE_DIRS_RELEASE}> APPEND)
set_property(TARGET websockets PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Release>:${libwebsockets_websockets_COMPILE_DEFINITIONS_RELEASE}> APPEND)
set_property(TARGET websockets PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Release>:
             ${libwebsockets_websockets_COMPILE_OPTIONS_C_RELEASE}
             ${libwebsockets_websockets_COMPILE_OPTIONS_CXX_RELEASE}> APPEND)
set(libwebsockets_websockets_TARGET_PROPERTIES TRUE)
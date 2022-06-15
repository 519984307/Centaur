########### VARIABLES #######################################################################
#############################################################################################

set(libwebsockets_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${libwebsockets_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${libwebsockets_COMPILE_OPTIONS_C_DEBUG}>")

set(libwebsockets_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libwebsockets_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libwebsockets_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libwebsockets_EXE_LINK_FLAGS_DEBUG}>")

set(libwebsockets_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(libwebsockets_FRAMEWORKS_FOUND_DEBUG "${libwebsockets_FRAMEWORKS_DEBUG}" "${libwebsockets_FRAMEWORK_DIRS_DEBUG}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_libwebsockets_DEPENDENCIES_DEBUG "${libwebsockets_FRAMEWORKS_FOUND_DEBUG} ${libwebsockets_SYSTEM_LIBS_DEBUG} openssl::openssl")

set(libwebsockets_LIBRARIES_TARGETS_DEBUG "") # Will be filled later
set(libwebsockets_LIBRARIES_DEBUG "") # Will be filled later
conan_package_library_targets("${libwebsockets_LIBS_DEBUG}"    # libraries
                              "${libwebsockets_LIB_DIRS_DEBUG}" # package_libdir
                              "${_libwebsockets_DEPENDENCIES_DEBUG}" # deps
                              libwebsockets_LIBRARIES_DEBUG   # out_libraries
                              libwebsockets_LIBRARIES_TARGETS_DEBUG  # out_libraries_targets
                              "_DEBUG"  # config_suffix
                              "libwebsockets")    # package_name

foreach(_FRAMEWORK ${libwebsockets_FRAMEWORKS_FOUND_DEBUG})
    list(APPEND libwebsockets_LIBRARIES_TARGETS_DEBUG ${_FRAMEWORK})
    list(APPEND libwebsockets_LIBRARIES_DEBUG ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${libwebsockets_SYSTEM_LIBS_DEBUG})
    list(APPEND libwebsockets_LIBRARIES_TARGETS_DEBUG ${_SYSTEM_LIB})
    list(APPEND libwebsockets_LIBRARIES_DEBUG ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(libwebsockets_LIBRARIES_TARGETS_DEBUG "${libwebsockets_LIBRARIES_TARGETS_DEBUG};openssl::openssl")
set(libwebsockets_LIBRARIES_DEBUG "${libwebsockets_LIBRARIES_DEBUG};openssl::openssl")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${libwebsockets_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${libwebsockets_BUILD_DIRS_DEBUG} ${CMAKE_PREFIX_PATH})
########## COMPONENT websockets FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(libwebsockets_websockets_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(libwebsockets_websockets_FRAMEWORKS_FOUND_DEBUG "${libwebsockets_websockets_FRAMEWORKS_DEBUG}" "${libwebsockets_websockets_FRAMEWORK_DIRS_DEBUG}")

set(libwebsockets_websockets_LIB_TARGETS_DEBUG "")
set(libwebsockets_websockets_NOT_USED_DEBUG "")
set(libwebsockets_websockets_LIBS_FRAMEWORKS_DEPS_DEBUG ${libwebsockets_websockets_FRAMEWORKS_FOUND_DEBUG} ${libwebsockets_websockets_SYSTEM_LIBS_DEBUG} ${libwebsockets_websockets_DEPENDENCIES_DEBUG})
conan_package_library_targets("${libwebsockets_websockets_LIBS_DEBUG}"
                              "${libwebsockets_websockets_LIB_DIRS_DEBUG}"
                              "${libwebsockets_websockets_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              libwebsockets_websockets_NOT_USED_DEBUG
                              libwebsockets_websockets_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "libwebsockets_websockets")

set(libwebsockets_websockets_LINK_LIBS_DEBUG ${libwebsockets_websockets_LIB_TARGETS_DEBUG} ${libwebsockets_websockets_LIBS_FRAMEWORKS_DEPS_DEBUG})


########## GLOBAL TARGET PROPERTIES Debug ########################################
set_property(TARGET websockets
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${libwebsockets_LIBRARIES_TARGETS_DEBUG}
                                           ${libwebsockets_OBJECTS_DEBUG}> APPEND)
set_property(TARGET websockets
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${libwebsockets_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET websockets
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${libwebsockets_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET websockets
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${libwebsockets_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET websockets
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:${libwebsockets_COMPILE_OPTIONS_DEBUG}> APPEND)

########## COMPONENTS TARGET PROPERTIES Debug ########################################
########## COMPONENT websockets TARGET PROPERTIES ######################################
set_property(TARGET websockets PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${libwebsockets_websockets_LINK_LIBS_DEBUG}
             ${libwebsockets_websockets_OBJECTS_DEBUG}> APPEND)
set_property(TARGET websockets PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${libwebsockets_websockets_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET websockets PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${libwebsockets_websockets_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET websockets PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${libwebsockets_websockets_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET websockets PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${libwebsockets_websockets_COMPILE_OPTIONS_C_DEBUG}
             ${libwebsockets_websockets_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(libwebsockets_websockets_TARGET_PROPERTIES TRUE)
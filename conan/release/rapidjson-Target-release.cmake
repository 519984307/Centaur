########### VARIABLES #######################################################################
#############################################################################################

set(rapidjson_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${rapidjson_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${rapidjson_COMPILE_OPTIONS_C_RELEASE}>")

set(rapidjson_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${rapidjson_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${rapidjson_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${rapidjson_EXE_LINK_FLAGS_RELEASE}>")

set(rapidjson_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(rapidjson_FRAMEWORKS_FOUND_RELEASE "${rapidjson_FRAMEWORKS_RELEASE}" "${rapidjson_FRAMEWORK_DIRS_RELEASE}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_rapidjson_DEPENDENCIES_RELEASE "${rapidjson_FRAMEWORKS_FOUND_RELEASE} ${rapidjson_SYSTEM_LIBS_RELEASE} ")

set(rapidjson_LIBRARIES_TARGETS_RELEASE "") # Will be filled later
set(rapidjson_LIBRARIES_RELEASE "") # Will be filled later
conan_package_library_targets("${rapidjson_LIBS_RELEASE}"    # libraries
                              "${rapidjson_LIB_DIRS_RELEASE}" # package_libdir
                              "${_rapidjson_DEPENDENCIES_RELEASE}" # deps
                              rapidjson_LIBRARIES_RELEASE   # out_libraries
                              rapidjson_LIBRARIES_TARGETS_RELEASE  # out_libraries_targets
                              "_RELEASE"  # config_suffix
                              "rapidjson")    # package_name

foreach(_FRAMEWORK ${rapidjson_FRAMEWORKS_FOUND_RELEASE})
    list(APPEND rapidjson_LIBRARIES_TARGETS_RELEASE ${_FRAMEWORK})
    list(APPEND rapidjson_LIBRARIES_RELEASE ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${rapidjson_SYSTEM_LIBS_RELEASE})
    list(APPEND rapidjson_LIBRARIES_TARGETS_RELEASE ${_SYSTEM_LIB})
    list(APPEND rapidjson_LIBRARIES_RELEASE ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(rapidjson_LIBRARIES_TARGETS_RELEASE "${rapidjson_LIBRARIES_TARGETS_RELEASE};")
set(rapidjson_LIBRARIES_RELEASE "${rapidjson_LIBRARIES_RELEASE};")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${rapidjson_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${rapidjson_BUILD_DIRS_RELEASE} ${CMAKE_PREFIX_PATH})


########## GLOBAL TARGET PROPERTIES Release ########################################
set_property(TARGET rapidjson::rapidjson
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${rapidjson_LIBRARIES_TARGETS_RELEASE}
                                           ${rapidjson_OBJECTS_RELEASE}> APPEND)
set_property(TARGET rapidjson::rapidjson
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Release>:${rapidjson_LINKER_FLAGS_RELEASE}> APPEND)
set_property(TARGET rapidjson::rapidjson
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Release>:${rapidjson_INCLUDE_DIRS_RELEASE}> APPEND)
set_property(TARGET rapidjson::rapidjson
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Release>:${rapidjson_COMPILE_DEFINITIONS_RELEASE}> APPEND)
set_property(TARGET rapidjson::rapidjson
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Release>:${rapidjson_COMPILE_OPTIONS_RELEASE}> APPEND)

########## COMPONENTS TARGET PROPERTIES Release ########################################
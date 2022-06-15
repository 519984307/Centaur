########### VARIABLES #######################################################################
#############################################################################################

set(rapidjson_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${rapidjson_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${rapidjson_COMPILE_OPTIONS_C_DEBUG}>")

set(rapidjson_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${rapidjson_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${rapidjson_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${rapidjson_EXE_LINK_FLAGS_DEBUG}>")

set(rapidjson_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(rapidjson_FRAMEWORKS_FOUND_DEBUG "${rapidjson_FRAMEWORKS_DEBUG}" "${rapidjson_FRAMEWORK_DIRS_DEBUG}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_rapidjson_DEPENDENCIES_DEBUG "${rapidjson_FRAMEWORKS_FOUND_DEBUG} ${rapidjson_SYSTEM_LIBS_DEBUG} ")

set(rapidjson_LIBRARIES_TARGETS_DEBUG "") # Will be filled later
set(rapidjson_LIBRARIES_DEBUG "") # Will be filled later
conan_package_library_targets("${rapidjson_LIBS_DEBUG}"    # libraries
                              "${rapidjson_LIB_DIRS_DEBUG}" # package_libdir
                              "${_rapidjson_DEPENDENCIES_DEBUG}" # deps
                              rapidjson_LIBRARIES_DEBUG   # out_libraries
                              rapidjson_LIBRARIES_TARGETS_DEBUG  # out_libraries_targets
                              "_DEBUG"  # config_suffix
                              "rapidjson")    # package_name

foreach(_FRAMEWORK ${rapidjson_FRAMEWORKS_FOUND_DEBUG})
    list(APPEND rapidjson_LIBRARIES_TARGETS_DEBUG ${_FRAMEWORK})
    list(APPEND rapidjson_LIBRARIES_DEBUG ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${rapidjson_SYSTEM_LIBS_DEBUG})
    list(APPEND rapidjson_LIBRARIES_TARGETS_DEBUG ${_SYSTEM_LIB})
    list(APPEND rapidjson_LIBRARIES_DEBUG ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(rapidjson_LIBRARIES_TARGETS_DEBUG "${rapidjson_LIBRARIES_TARGETS_DEBUG};")
set(rapidjson_LIBRARIES_DEBUG "${rapidjson_LIBRARIES_DEBUG};")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${rapidjson_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${rapidjson_BUILD_DIRS_DEBUG} ${CMAKE_PREFIX_PATH})


########## GLOBAL TARGET PROPERTIES Debug ########################################
set_property(TARGET rapidjson::rapidjson
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${rapidjson_LIBRARIES_TARGETS_DEBUG}
                                           ${rapidjson_OBJECTS_DEBUG}> APPEND)
set_property(TARGET rapidjson::rapidjson
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${rapidjson_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET rapidjson::rapidjson
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${rapidjson_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET rapidjson::rapidjson
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${rapidjson_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET rapidjson::rapidjson
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:${rapidjson_COMPILE_OPTIONS_DEBUG}> APPEND)

########## COMPONENTS TARGET PROPERTIES Debug ########################################
########### VARIABLES #######################################################################
#############################################################################################

set(cpr_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${cpr_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${cpr_COMPILE_OPTIONS_C_DEBUG}>")

set(cpr_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${cpr_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${cpr_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${cpr_EXE_LINK_FLAGS_DEBUG}>")

set(cpr_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(cpr_FRAMEWORKS_FOUND_DEBUG "${cpr_FRAMEWORKS_DEBUG}" "${cpr_FRAMEWORK_DIRS_DEBUG}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_cpr_DEPENDENCIES_DEBUG "${cpr_FRAMEWORKS_FOUND_DEBUG} ${cpr_SYSTEM_LIBS_DEBUG} CURL::libcurl")

set(cpr_LIBRARIES_TARGETS_DEBUG "") # Will be filled later
set(cpr_LIBRARIES_DEBUG "") # Will be filled later
conan_package_library_targets("${cpr_LIBS_DEBUG}"    # libraries
                              "${cpr_LIB_DIRS_DEBUG}" # package_libdir
                              "${_cpr_DEPENDENCIES_DEBUG}" # deps
                              cpr_LIBRARIES_DEBUG   # out_libraries
                              cpr_LIBRARIES_TARGETS_DEBUG  # out_libraries_targets
                              "_DEBUG"  # config_suffix
                              "cpr")    # package_name

foreach(_FRAMEWORK ${cpr_FRAMEWORKS_FOUND_DEBUG})
    list(APPEND cpr_LIBRARIES_TARGETS_DEBUG ${_FRAMEWORK})
    list(APPEND cpr_LIBRARIES_DEBUG ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${cpr_SYSTEM_LIBS_DEBUG})
    list(APPEND cpr_LIBRARIES_TARGETS_DEBUG ${_SYSTEM_LIB})
    list(APPEND cpr_LIBRARIES_DEBUG ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(cpr_LIBRARIES_TARGETS_DEBUG "${cpr_LIBRARIES_TARGETS_DEBUG};CURL::libcurl")
set(cpr_LIBRARIES_DEBUG "${cpr_LIBRARIES_DEBUG};CURL::libcurl")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${cpr_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${cpr_BUILD_DIRS_DEBUG} ${CMAKE_PREFIX_PATH})


########## GLOBAL TARGET PROPERTIES Debug ########################################
set_property(TARGET cpr::cpr
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${cpr_LIBRARIES_TARGETS_DEBUG}
                                           ${cpr_OBJECTS_DEBUG}> APPEND)
set_property(TARGET cpr::cpr
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${cpr_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET cpr::cpr
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${cpr_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET cpr::cpr
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${cpr_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET cpr::cpr
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:${cpr_COMPILE_OPTIONS_DEBUG}> APPEND)

########## COMPONENTS TARGET PROPERTIES Debug ########################################
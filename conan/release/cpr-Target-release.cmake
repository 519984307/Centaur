########### VARIABLES #######################################################################
#############################################################################################

set(cpr_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${cpr_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${cpr_COMPILE_OPTIONS_C_RELEASE}>")

set(cpr_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${cpr_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${cpr_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${cpr_EXE_LINK_FLAGS_RELEASE}>")

set(cpr_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(cpr_FRAMEWORKS_FOUND_RELEASE "${cpr_FRAMEWORKS_RELEASE}" "${cpr_FRAMEWORK_DIRS_RELEASE}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_cpr_DEPENDENCIES_RELEASE "${cpr_FRAMEWORKS_FOUND_RELEASE} ${cpr_SYSTEM_LIBS_RELEASE} CURL::libcurl")

set(cpr_LIBRARIES_TARGETS_RELEASE "") # Will be filled later
set(cpr_LIBRARIES_RELEASE "") # Will be filled later
conan_package_library_targets("${cpr_LIBS_RELEASE}"    # libraries
                              "${cpr_LIB_DIRS_RELEASE}" # package_libdir
                              "${_cpr_DEPENDENCIES_RELEASE}" # deps
                              cpr_LIBRARIES_RELEASE   # out_libraries
                              cpr_LIBRARIES_TARGETS_RELEASE  # out_libraries_targets
                              "_RELEASE"  # config_suffix
                              "cpr")    # package_name

foreach(_FRAMEWORK ${cpr_FRAMEWORKS_FOUND_RELEASE})
    list(APPEND cpr_LIBRARIES_TARGETS_RELEASE ${_FRAMEWORK})
    list(APPEND cpr_LIBRARIES_RELEASE ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${cpr_SYSTEM_LIBS_RELEASE})
    list(APPEND cpr_LIBRARIES_TARGETS_RELEASE ${_SYSTEM_LIB})
    list(APPEND cpr_LIBRARIES_RELEASE ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(cpr_LIBRARIES_TARGETS_RELEASE "${cpr_LIBRARIES_TARGETS_RELEASE};CURL::libcurl")
set(cpr_LIBRARIES_RELEASE "${cpr_LIBRARIES_RELEASE};CURL::libcurl")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${cpr_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${cpr_BUILD_DIRS_RELEASE} ${CMAKE_PREFIX_PATH})


########## GLOBAL TARGET PROPERTIES Release ########################################
set_property(TARGET cpr::cpr
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${cpr_LIBRARIES_TARGETS_RELEASE}
                                           ${cpr_OBJECTS_RELEASE}> APPEND)
set_property(TARGET cpr::cpr
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Release>:${cpr_LINKER_FLAGS_RELEASE}> APPEND)
set_property(TARGET cpr::cpr
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Release>:${cpr_INCLUDE_DIRS_RELEASE}> APPEND)
set_property(TARGET cpr::cpr
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Release>:${cpr_COMPILE_DEFINITIONS_RELEASE}> APPEND)
set_property(TARGET cpr::cpr
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Release>:${cpr_COMPILE_OPTIONS_RELEASE}> APPEND)

########## COMPONENTS TARGET PROPERTIES Release ########################################
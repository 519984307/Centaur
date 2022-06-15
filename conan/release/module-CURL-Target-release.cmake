########### VARIABLES #######################################################################
#############################################################################################

set(libcurl_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${libcurl_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${libcurl_COMPILE_OPTIONS_C_RELEASE}>")

set(libcurl_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libcurl_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libcurl_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libcurl_EXE_LINK_FLAGS_RELEASE}>")

set(libcurl_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(libcurl_FRAMEWORKS_FOUND_RELEASE "${libcurl_FRAMEWORKS_RELEASE}" "${libcurl_FRAMEWORK_DIRS_RELEASE}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_libcurl_DEPENDENCIES_RELEASE "${libcurl_FRAMEWORKS_FOUND_RELEASE} ${libcurl_SYSTEM_LIBS_RELEASE} ZLIB::ZLIB")

set(libcurl_LIBRARIES_TARGETS_RELEASE "") # Will be filled later
set(libcurl_LIBRARIES_RELEASE "") # Will be filled later
conan_package_library_targets("${libcurl_LIBS_RELEASE}"    # libraries
                              "${libcurl_LIB_DIRS_RELEASE}" # package_libdir
                              "${_libcurl_DEPENDENCIES_RELEASE}" # deps
                              libcurl_LIBRARIES_RELEASE   # out_libraries
                              libcurl_LIBRARIES_TARGETS_RELEASE  # out_libraries_targets
                              "_RELEASE"  # config_suffix
                              "libcurl")    # package_name

foreach(_FRAMEWORK ${libcurl_FRAMEWORKS_FOUND_RELEASE})
    list(APPEND libcurl_LIBRARIES_TARGETS_RELEASE ${_FRAMEWORK})
    list(APPEND libcurl_LIBRARIES_RELEASE ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${libcurl_SYSTEM_LIBS_RELEASE})
    list(APPEND libcurl_LIBRARIES_TARGETS_RELEASE ${_SYSTEM_LIB})
    list(APPEND libcurl_LIBRARIES_RELEASE ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(libcurl_LIBRARIES_TARGETS_RELEASE "${libcurl_LIBRARIES_TARGETS_RELEASE};ZLIB::ZLIB")
set(libcurl_LIBRARIES_RELEASE "${libcurl_LIBRARIES_RELEASE};ZLIB::ZLIB")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${libcurl_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${libcurl_BUILD_DIRS_RELEASE} ${CMAKE_PREFIX_PATH})
########## COMPONENT CURL::libcurl FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(libcurl_CURL_libcurl_FRAMEWORKS_FOUND_RELEASE "")
conan_find_apple_frameworks(libcurl_CURL_libcurl_FRAMEWORKS_FOUND_RELEASE "${libcurl_CURL_libcurl_FRAMEWORKS_RELEASE}" "${libcurl_CURL_libcurl_FRAMEWORK_DIRS_RELEASE}")

set(libcurl_CURL_libcurl_LIB_TARGETS_RELEASE "")
set(libcurl_CURL_libcurl_NOT_USED_RELEASE "")
set(libcurl_CURL_libcurl_LIBS_FRAMEWORKS_DEPS_RELEASE ${libcurl_CURL_libcurl_FRAMEWORKS_FOUND_RELEASE} ${libcurl_CURL_libcurl_SYSTEM_LIBS_RELEASE} ${libcurl_CURL_libcurl_DEPENDENCIES_RELEASE})
conan_package_library_targets("${libcurl_CURL_libcurl_LIBS_RELEASE}"
                              "${libcurl_CURL_libcurl_LIB_DIRS_RELEASE}"
                              "${libcurl_CURL_libcurl_LIBS_FRAMEWORKS_DEPS_RELEASE}"
                              libcurl_CURL_libcurl_NOT_USED_RELEASE
                              libcurl_CURL_libcurl_LIB_TARGETS_RELEASE
                              "_RELEASE"
                              "libcurl_CURL_libcurl")

set(libcurl_CURL_libcurl_LINK_LIBS_RELEASE ${libcurl_CURL_libcurl_LIB_TARGETS_RELEASE} ${libcurl_CURL_libcurl_LIBS_FRAMEWORKS_DEPS_RELEASE})


########## GLOBAL TARGET PROPERTIES Release ########################################
set_property(TARGET CURL::libcurl
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${libcurl_LIBRARIES_TARGETS_RELEASE}
                                           ${libcurl_OBJECTS_RELEASE}> APPEND)
set_property(TARGET CURL::libcurl
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Release>:${libcurl_LINKER_FLAGS_RELEASE}> APPEND)
set_property(TARGET CURL::libcurl
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Release>:${libcurl_INCLUDE_DIRS_RELEASE}> APPEND)
set_property(TARGET CURL::libcurl
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Release>:${libcurl_COMPILE_DEFINITIONS_RELEASE}> APPEND)
set_property(TARGET CURL::libcurl
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Release>:${libcurl_COMPILE_OPTIONS_RELEASE}> APPEND)

########## COMPONENTS TARGET PROPERTIES Release ########################################
########## COMPONENT CURL::libcurl TARGET PROPERTIES ######################################
set_property(TARGET CURL::libcurl PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${libcurl_CURL_libcurl_LINK_LIBS_RELEASE}
             ${libcurl_CURL_libcurl_OBJECTS_RELEASE}> APPEND)
set_property(TARGET CURL::libcurl PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Release>:${libcurl_CURL_libcurl_LINKER_FLAGS_RELEASE}> APPEND)
set_property(TARGET CURL::libcurl PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Release>:${libcurl_CURL_libcurl_INCLUDE_DIRS_RELEASE}> APPEND)
set_property(TARGET CURL::libcurl PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Release>:${libcurl_CURL_libcurl_COMPILE_DEFINITIONS_RELEASE}> APPEND)
set_property(TARGET CURL::libcurl PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Release>:
             ${libcurl_CURL_libcurl_COMPILE_OPTIONS_C_RELEASE}
             ${libcurl_CURL_libcurl_COMPILE_OPTIONS_CXX_RELEASE}> APPEND)
set(libcurl_CURL_libcurl_TARGET_PROPERTIES TRUE)
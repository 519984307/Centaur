########### VARIABLES #######################################################################
#############################################################################################

set(libcurl_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${libcurl_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${libcurl_COMPILE_OPTIONS_C_DEBUG}>")

set(libcurl_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libcurl_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libcurl_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libcurl_EXE_LINK_FLAGS_DEBUG}>")

set(libcurl_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(libcurl_FRAMEWORKS_FOUND_DEBUG "${libcurl_FRAMEWORKS_DEBUG}" "${libcurl_FRAMEWORK_DIRS_DEBUG}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_libcurl_DEPENDENCIES_DEBUG "${libcurl_FRAMEWORKS_FOUND_DEBUG} ${libcurl_SYSTEM_LIBS_DEBUG} ZLIB::ZLIB")

set(libcurl_LIBRARIES_TARGETS_DEBUG "") # Will be filled later
set(libcurl_LIBRARIES_DEBUG "") # Will be filled later
conan_package_library_targets("${libcurl_LIBS_DEBUG}"    # libraries
                              "${libcurl_LIB_DIRS_DEBUG}" # package_libdir
                              "${_libcurl_DEPENDENCIES_DEBUG}" # deps
                              libcurl_LIBRARIES_DEBUG   # out_libraries
                              libcurl_LIBRARIES_TARGETS_DEBUG  # out_libraries_targets
                              "_DEBUG"  # config_suffix
                              "libcurl")    # package_name

foreach(_FRAMEWORK ${libcurl_FRAMEWORKS_FOUND_DEBUG})
    list(APPEND libcurl_LIBRARIES_TARGETS_DEBUG ${_FRAMEWORK})
    list(APPEND libcurl_LIBRARIES_DEBUG ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${libcurl_SYSTEM_LIBS_DEBUG})
    list(APPEND libcurl_LIBRARIES_TARGETS_DEBUG ${_SYSTEM_LIB})
    list(APPEND libcurl_LIBRARIES_DEBUG ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(libcurl_LIBRARIES_TARGETS_DEBUG "${libcurl_LIBRARIES_TARGETS_DEBUG};ZLIB::ZLIB")
set(libcurl_LIBRARIES_DEBUG "${libcurl_LIBRARIES_DEBUG};ZLIB::ZLIB")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${libcurl_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${libcurl_BUILD_DIRS_DEBUG} ${CMAKE_PREFIX_PATH})
########## COMPONENT CURL::libcurl FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(libcurl_CURL_libcurl_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(libcurl_CURL_libcurl_FRAMEWORKS_FOUND_DEBUG "${libcurl_CURL_libcurl_FRAMEWORKS_DEBUG}" "${libcurl_CURL_libcurl_FRAMEWORK_DIRS_DEBUG}")

set(libcurl_CURL_libcurl_LIB_TARGETS_DEBUG "")
set(libcurl_CURL_libcurl_NOT_USED_DEBUG "")
set(libcurl_CURL_libcurl_LIBS_FRAMEWORKS_DEPS_DEBUG ${libcurl_CURL_libcurl_FRAMEWORKS_FOUND_DEBUG} ${libcurl_CURL_libcurl_SYSTEM_LIBS_DEBUG} ${libcurl_CURL_libcurl_DEPENDENCIES_DEBUG})
conan_package_library_targets("${libcurl_CURL_libcurl_LIBS_DEBUG}"
                              "${libcurl_CURL_libcurl_LIB_DIRS_DEBUG}"
                              "${libcurl_CURL_libcurl_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              libcurl_CURL_libcurl_NOT_USED_DEBUG
                              libcurl_CURL_libcurl_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "libcurl_CURL_libcurl")

set(libcurl_CURL_libcurl_LINK_LIBS_DEBUG ${libcurl_CURL_libcurl_LIB_TARGETS_DEBUG} ${libcurl_CURL_libcurl_LIBS_FRAMEWORKS_DEPS_DEBUG})


########## GLOBAL TARGET PROPERTIES Debug ########################################
set_property(TARGET CURL::libcurl
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${libcurl_LIBRARIES_TARGETS_DEBUG}
                                           ${libcurl_OBJECTS_DEBUG}> APPEND)
set_property(TARGET CURL::libcurl
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${libcurl_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET CURL::libcurl
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${libcurl_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET CURL::libcurl
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${libcurl_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET CURL::libcurl
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:${libcurl_COMPILE_OPTIONS_DEBUG}> APPEND)

########## COMPONENTS TARGET PROPERTIES Debug ########################################
########## COMPONENT CURL::libcurl TARGET PROPERTIES ######################################
set_property(TARGET CURL::libcurl PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${libcurl_CURL_libcurl_LINK_LIBS_DEBUG}
             ${libcurl_CURL_libcurl_OBJECTS_DEBUG}> APPEND)
set_property(TARGET CURL::libcurl PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${libcurl_CURL_libcurl_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET CURL::libcurl PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${libcurl_CURL_libcurl_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET CURL::libcurl PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${libcurl_CURL_libcurl_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET CURL::libcurl PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${libcurl_CURL_libcurl_COMPILE_OPTIONS_C_DEBUG}
             ${libcurl_CURL_libcurl_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(libcurl_CURL_libcurl_TARGET_PROPERTIES TRUE)
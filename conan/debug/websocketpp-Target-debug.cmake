########### VARIABLES #######################################################################
#############################################################################################

set(websocketpp_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${websocketpp_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${websocketpp_COMPILE_OPTIONS_C_DEBUG}>")

set(websocketpp_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${websocketpp_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${websocketpp_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${websocketpp_EXE_LINK_FLAGS_DEBUG}>")

set(websocketpp_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(websocketpp_FRAMEWORKS_FOUND_DEBUG "${websocketpp_FRAMEWORKS_DEBUG}" "${websocketpp_FRAMEWORK_DIRS_DEBUG}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_websocketpp_DEPENDENCIES_DEBUG "${websocketpp_FRAMEWORKS_FOUND_DEBUG} ${websocketpp_SYSTEM_LIBS_DEBUG} openssl::openssl;ZLIB::ZLIB;boost::boost")

set(websocketpp_LIBRARIES_TARGETS_DEBUG "") # Will be filled later
set(websocketpp_LIBRARIES_DEBUG "") # Will be filled later
conan_package_library_targets("${websocketpp_LIBS_DEBUG}"    # libraries
                              "${websocketpp_LIB_DIRS_DEBUG}" # package_libdir
                              "${_websocketpp_DEPENDENCIES_DEBUG}" # deps
                              websocketpp_LIBRARIES_DEBUG   # out_libraries
                              websocketpp_LIBRARIES_TARGETS_DEBUG  # out_libraries_targets
                              "_DEBUG"  # config_suffix
                              "websocketpp")    # package_name

foreach(_FRAMEWORK ${websocketpp_FRAMEWORKS_FOUND_DEBUG})
    list(APPEND websocketpp_LIBRARIES_TARGETS_DEBUG ${_FRAMEWORK})
    list(APPEND websocketpp_LIBRARIES_DEBUG ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${websocketpp_SYSTEM_LIBS_DEBUG})
    list(APPEND websocketpp_LIBRARIES_TARGETS_DEBUG ${_SYSTEM_LIB})
    list(APPEND websocketpp_LIBRARIES_DEBUG ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(websocketpp_LIBRARIES_TARGETS_DEBUG "${websocketpp_LIBRARIES_TARGETS_DEBUG};openssl::openssl;ZLIB::ZLIB;boost::boost")
set(websocketpp_LIBRARIES_DEBUG "${websocketpp_LIBRARIES_DEBUG};openssl::openssl;ZLIB::ZLIB;boost::boost")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${websocketpp_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${websocketpp_BUILD_DIRS_DEBUG} ${CMAKE_PREFIX_PATH})


########## GLOBAL TARGET PROPERTIES Debug ########################################
set_property(TARGET websocketpp::websocketpp
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${websocketpp_LIBRARIES_TARGETS_DEBUG}
                                           ${websocketpp_OBJECTS_DEBUG}> APPEND)
set_property(TARGET websocketpp::websocketpp
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${websocketpp_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET websocketpp::websocketpp
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${websocketpp_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET websocketpp::websocketpp
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${websocketpp_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET websocketpp::websocketpp
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:${websocketpp_COMPILE_OPTIONS_DEBUG}> APPEND)

########## COMPONENTS TARGET PROPERTIES Debug ########################################
########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

list(APPEND libwebsockets_COMPONENT_NAMES websockets)
list(REMOVE_DUPLICATES libwebsockets_COMPONENT_NAMES)
list(APPEND libwebsockets_FIND_DEPENDENCY_NAMES OpenSSL)
list(REMOVE_DUPLICATES libwebsockets_FIND_DEPENDENCY_NAMES)
set(OpenSSL_FIND_MODE "NO_MODULE")

########### VARIABLES #######################################################################
#############################################################################################
set(libwebsockets_PACKAGE_FOLDER_DEBUG "/Users/ricardo/.conan/data/libwebsockets/4.3.0/_/_/package/c7d7e09f53f1db9545a5c8ccd087c20889773a20")
set(libwebsockets_INCLUDE_DIRS_DEBUG "${libwebsockets_PACKAGE_FOLDER_DEBUG}/include")
set(libwebsockets_RES_DIRS_DEBUG "${libwebsockets_PACKAGE_FOLDER_DEBUG}/res")
set(libwebsockets_DEFINITIONS_DEBUG )
set(libwebsockets_SHARED_LINK_FLAGS_DEBUG )
set(libwebsockets_EXE_LINK_FLAGS_DEBUG )
set(libwebsockets_OBJECTS_DEBUG )
set(libwebsockets_COMPILE_DEFINITIONS_DEBUG )
set(libwebsockets_COMPILE_OPTIONS_C_DEBUG )
set(libwebsockets_COMPILE_OPTIONS_CXX_DEBUG )
set(libwebsockets_LIB_DIRS_DEBUG "${libwebsockets_PACKAGE_FOLDER_DEBUG}/lib")
set(libwebsockets_LIBS_DEBUG websockets)
set(libwebsockets_SYSTEM_LIBS_DEBUG )
set(libwebsockets_FRAMEWORK_DIRS_DEBUG "${libwebsockets_PACKAGE_FOLDER_DEBUG}/Frameworks")
set(libwebsockets_FRAMEWORKS_DEBUG )
set(libwebsockets_BUILD_MODULES_PATHS_DEBUG )
set(libwebsockets_BUILD_DIRS_DEBUG "${libwebsockets_PACKAGE_FOLDER_DEBUG}/")

set(libwebsockets_COMPONENTS_DEBUG websockets)
########### COMPONENT websockets VARIABLES #############################################
set(libwebsockets_websockets_INCLUDE_DIRS_DEBUG "${libwebsockets_PACKAGE_FOLDER_DEBUG}/include")
set(libwebsockets_websockets_LIB_DIRS_DEBUG "${libwebsockets_PACKAGE_FOLDER_DEBUG}/lib")
set(libwebsockets_websockets_RES_DIRS_DEBUG "${libwebsockets_PACKAGE_FOLDER_DEBUG}/res")
set(libwebsockets_websockets_DEFINITIONS_DEBUG )
set(libwebsockets_websockets_OBJECTS_DEBUG )
set(libwebsockets_websockets_COMPILE_DEFINITIONS_DEBUG )
set(libwebsockets_websockets_COMPILE_OPTIONS_C_DEBUG "")
set(libwebsockets_websockets_COMPILE_OPTIONS_CXX_DEBUG "")
set(libwebsockets_websockets_LIBS_DEBUG websockets)
set(libwebsockets_websockets_SYSTEM_LIBS_DEBUG )
set(libwebsockets_websockets_FRAMEWORK_DIRS_DEBUG "${libwebsockets_PACKAGE_FOLDER_DEBUG}/Frameworks")
set(libwebsockets_websockets_FRAMEWORKS_DEBUG )
set(libwebsockets_websockets_DEPENDENCIES_DEBUG openssl::openssl)
set(libwebsockets_websockets_SHARED_LINK_FLAGS_DEBUG )
set(libwebsockets_websockets_EXE_LINK_FLAGS_DEBUG )
set(libwebsockets_websockets_LINKER_FLAGS_DEBUG
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libwebsockets_websockets_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libwebsockets_websockets_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libwebsockets_websockets_EXE_LINK_FLAGS_DEBUG}>
)
list(APPEND libwebsockets_BUILD_MODULES_PATHS_DEBUG )
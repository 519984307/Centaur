########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

list(APPEND libwebsockets_COMPONENT_NAMES websockets)
list(REMOVE_DUPLICATES libwebsockets_COMPONENT_NAMES)
list(APPEND libwebsockets_FIND_DEPENDENCY_NAMES OpenSSL)
list(REMOVE_DUPLICATES libwebsockets_FIND_DEPENDENCY_NAMES)
set(OpenSSL_FIND_MODE "NO_MODULE")

########### VARIABLES #######################################################################
#############################################################################################
set(libwebsockets_PACKAGE_FOLDER_RELEASE "/Users/ricardo/.conan/data/libwebsockets/4.3.0/_/_/package/48633482216d06ba409ce622d84c996dffe0a0c8")
set(libwebsockets_INCLUDE_DIRS_RELEASE "${libwebsockets_PACKAGE_FOLDER_RELEASE}/include")
set(libwebsockets_RES_DIRS_RELEASE "${libwebsockets_PACKAGE_FOLDER_RELEASE}/res")
set(libwebsockets_DEFINITIONS_RELEASE )
set(libwebsockets_SHARED_LINK_FLAGS_RELEASE )
set(libwebsockets_EXE_LINK_FLAGS_RELEASE )
set(libwebsockets_OBJECTS_RELEASE )
set(libwebsockets_COMPILE_DEFINITIONS_RELEASE )
set(libwebsockets_COMPILE_OPTIONS_C_RELEASE )
set(libwebsockets_COMPILE_OPTIONS_CXX_RELEASE )
set(libwebsockets_LIB_DIRS_RELEASE "${libwebsockets_PACKAGE_FOLDER_RELEASE}/lib")
set(libwebsockets_LIBS_RELEASE websockets)
set(libwebsockets_SYSTEM_LIBS_RELEASE )
set(libwebsockets_FRAMEWORK_DIRS_RELEASE "${libwebsockets_PACKAGE_FOLDER_RELEASE}/Frameworks")
set(libwebsockets_FRAMEWORKS_RELEASE )
set(libwebsockets_BUILD_MODULES_PATHS_RELEASE )
set(libwebsockets_BUILD_DIRS_RELEASE "${libwebsockets_PACKAGE_FOLDER_RELEASE}/")

set(libwebsockets_COMPONENTS_RELEASE websockets)
########### COMPONENT websockets VARIABLES #############################################
set(libwebsockets_websockets_INCLUDE_DIRS_RELEASE "${libwebsockets_PACKAGE_FOLDER_RELEASE}/include")
set(libwebsockets_websockets_LIB_DIRS_RELEASE "${libwebsockets_PACKAGE_FOLDER_RELEASE}/lib")
set(libwebsockets_websockets_RES_DIRS_RELEASE "${libwebsockets_PACKAGE_FOLDER_RELEASE}/res")
set(libwebsockets_websockets_DEFINITIONS_RELEASE )
set(libwebsockets_websockets_OBJECTS_RELEASE )
set(libwebsockets_websockets_COMPILE_DEFINITIONS_RELEASE )
set(libwebsockets_websockets_COMPILE_OPTIONS_C_RELEASE "")
set(libwebsockets_websockets_COMPILE_OPTIONS_CXX_RELEASE "")
set(libwebsockets_websockets_LIBS_RELEASE websockets)
set(libwebsockets_websockets_SYSTEM_LIBS_RELEASE )
set(libwebsockets_websockets_FRAMEWORK_DIRS_RELEASE "${libwebsockets_PACKAGE_FOLDER_RELEASE}/Frameworks")
set(libwebsockets_websockets_FRAMEWORKS_RELEASE )
set(libwebsockets_websockets_DEPENDENCIES_RELEASE openssl::openssl)
set(libwebsockets_websockets_SHARED_LINK_FLAGS_RELEASE )
set(libwebsockets_websockets_EXE_LINK_FLAGS_RELEASE )
set(libwebsockets_websockets_LINKER_FLAGS_RELEASE
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libwebsockets_websockets_SHARED_LINK_FLAGS_RELEASE}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libwebsockets_websockets_SHARED_LINK_FLAGS_RELEASE}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libwebsockets_websockets_EXE_LINK_FLAGS_RELEASE}>
)
list(APPEND libwebsockets_BUILD_MODULES_PATHS_RELEASE )
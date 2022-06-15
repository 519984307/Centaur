########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

list(APPEND libcurl_COMPONENT_NAMES CURL::libcurl)
list(REMOVE_DUPLICATES libcurl_COMPONENT_NAMES)
list(APPEND libcurl_FIND_DEPENDENCY_NAMES ZLIB)
list(REMOVE_DUPLICATES libcurl_FIND_DEPENDENCY_NAMES)
set(ZLIB_FIND_MODE "NO_MODULE")

########### VARIABLES #######################################################################
#############################################################################################
set(libcurl_PACKAGE_FOLDER_DEBUG "/Users/ricardo/.conan/data/libcurl/7.80.0/_/_/package/63a1fd6bb0025ff45e6d194ce99323758991cb09")
set(libcurl_INCLUDE_DIRS_DEBUG "${libcurl_PACKAGE_FOLDER_DEBUG}/include")
set(libcurl_RES_DIRS_DEBUG "${libcurl_PACKAGE_FOLDER_DEBUG}/res")
set(libcurl_DEFINITIONS_DEBUG "-DCURL_STATICLIB=1")
set(libcurl_SHARED_LINK_FLAGS_DEBUG )
set(libcurl_EXE_LINK_FLAGS_DEBUG )
set(libcurl_OBJECTS_DEBUG )
set(libcurl_COMPILE_DEFINITIONS_DEBUG "CURL_STATICLIB=1")
set(libcurl_COMPILE_OPTIONS_C_DEBUG )
set(libcurl_COMPILE_OPTIONS_CXX_DEBUG )
set(libcurl_LIB_DIRS_DEBUG "${libcurl_PACKAGE_FOLDER_DEBUG}/lib")
set(libcurl_LIBS_DEBUG curl)
set(libcurl_SYSTEM_LIBS_DEBUG )
set(libcurl_FRAMEWORK_DIRS_DEBUG "${libcurl_PACKAGE_FOLDER_DEBUG}/Frameworks")
set(libcurl_FRAMEWORKS_DEBUG SystemConfiguration CoreFoundation Security)
set(libcurl_BUILD_MODULES_PATHS_DEBUG )
set(libcurl_BUILD_DIRS_DEBUG "${libcurl_PACKAGE_FOLDER_DEBUG}/")

set(libcurl_COMPONENTS_DEBUG CURL::libcurl)
########### COMPONENT CURL::libcurl VARIABLES #############################################
set(libcurl_CURL_libcurl_INCLUDE_DIRS_DEBUG "${libcurl_PACKAGE_FOLDER_DEBUG}/include")
set(libcurl_CURL_libcurl_LIB_DIRS_DEBUG "${libcurl_PACKAGE_FOLDER_DEBUG}/lib")
set(libcurl_CURL_libcurl_RES_DIRS_DEBUG "${libcurl_PACKAGE_FOLDER_DEBUG}/res")
set(libcurl_CURL_libcurl_DEFINITIONS_DEBUG "-DCURL_STATICLIB=1")
set(libcurl_CURL_libcurl_OBJECTS_DEBUG )
set(libcurl_CURL_libcurl_COMPILE_DEFINITIONS_DEBUG "CURL_STATICLIB=1")
set(libcurl_CURL_libcurl_COMPILE_OPTIONS_C_DEBUG "")
set(libcurl_CURL_libcurl_COMPILE_OPTIONS_CXX_DEBUG "")
set(libcurl_CURL_libcurl_LIBS_DEBUG curl)
set(libcurl_CURL_libcurl_SYSTEM_LIBS_DEBUG )
set(libcurl_CURL_libcurl_FRAMEWORK_DIRS_DEBUG "${libcurl_PACKAGE_FOLDER_DEBUG}/Frameworks")
set(libcurl_CURL_libcurl_FRAMEWORKS_DEBUG SystemConfiguration CoreFoundation Security)
set(libcurl_CURL_libcurl_DEPENDENCIES_DEBUG ZLIB::ZLIB)
set(libcurl_CURL_libcurl_SHARED_LINK_FLAGS_DEBUG )
set(libcurl_CURL_libcurl_EXE_LINK_FLAGS_DEBUG )
set(libcurl_CURL_libcurl_LINKER_FLAGS_DEBUG
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libcurl_CURL_libcurl_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libcurl_CURL_libcurl_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libcurl_CURL_libcurl_EXE_LINK_FLAGS_DEBUG}>
)
list(APPEND libcurl_BUILD_MODULES_PATHS_DEBUG )
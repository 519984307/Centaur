########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

list(APPEND sqlite3_COMPONENT_NAMES SQLite::SQLite3)
list(REMOVE_DUPLICATES sqlite3_COMPONENT_NAMES)
set(sqlite3_FIND_DEPENDENCY_NAMES "")

########### VARIABLES #######################################################################
#############################################################################################
set(sqlite3_PACKAGE_FOLDER_RELEASE "/Users/ricardo/.conan/data/sqlite3/3.38.5/_/_/package/105a8d60f8f74509af4922daa14b32b1e2ace8ac")
set(sqlite3_INCLUDE_DIRS_RELEASE "${sqlite3_PACKAGE_FOLDER_RELEASE}/include")
set(sqlite3_RES_DIRS_RELEASE "${sqlite3_PACKAGE_FOLDER_RELEASE}/res")
set(sqlite3_DEFINITIONS_RELEASE )
set(sqlite3_SHARED_LINK_FLAGS_RELEASE )
set(sqlite3_EXE_LINK_FLAGS_RELEASE )
set(sqlite3_OBJECTS_RELEASE )
set(sqlite3_COMPILE_DEFINITIONS_RELEASE )
set(sqlite3_COMPILE_OPTIONS_C_RELEASE )
set(sqlite3_COMPILE_OPTIONS_CXX_RELEASE )
set(sqlite3_LIB_DIRS_RELEASE "${sqlite3_PACKAGE_FOLDER_RELEASE}/lib")
set(sqlite3_LIBS_RELEASE sqlite3)
set(sqlite3_SYSTEM_LIBS_RELEASE )
set(sqlite3_FRAMEWORK_DIRS_RELEASE "${sqlite3_PACKAGE_FOLDER_RELEASE}/Frameworks")
set(sqlite3_FRAMEWORKS_RELEASE )
set(sqlite3_BUILD_MODULES_PATHS_RELEASE )
set(sqlite3_BUILD_DIRS_RELEASE "${sqlite3_PACKAGE_FOLDER_RELEASE}/")

set(sqlite3_COMPONENTS_RELEASE SQLite::SQLite3)
########### COMPONENT SQLite::SQLite3 VARIABLES #############################################
set(sqlite3_SQLite_SQLite3_INCLUDE_DIRS_RELEASE "${sqlite3_PACKAGE_FOLDER_RELEASE}/include")
set(sqlite3_SQLite_SQLite3_LIB_DIRS_RELEASE "${sqlite3_PACKAGE_FOLDER_RELEASE}/lib")
set(sqlite3_SQLite_SQLite3_RES_DIRS_RELEASE "${sqlite3_PACKAGE_FOLDER_RELEASE}/res")
set(sqlite3_SQLite_SQLite3_DEFINITIONS_RELEASE )
set(sqlite3_SQLite_SQLite3_OBJECTS_RELEASE )
set(sqlite3_SQLite_SQLite3_COMPILE_DEFINITIONS_RELEASE )
set(sqlite3_SQLite_SQLite3_COMPILE_OPTIONS_C_RELEASE "")
set(sqlite3_SQLite_SQLite3_COMPILE_OPTIONS_CXX_RELEASE "")
set(sqlite3_SQLite_SQLite3_LIBS_RELEASE sqlite3)
set(sqlite3_SQLite_SQLite3_SYSTEM_LIBS_RELEASE )
set(sqlite3_SQLite_SQLite3_FRAMEWORK_DIRS_RELEASE "${sqlite3_PACKAGE_FOLDER_RELEASE}/Frameworks")
set(sqlite3_SQLite_SQLite3_FRAMEWORKS_RELEASE )
set(sqlite3_SQLite_SQLite3_DEPENDENCIES_RELEASE )
set(sqlite3_SQLite_SQLite3_SHARED_LINK_FLAGS_RELEASE )
set(sqlite3_SQLite_SQLite3_EXE_LINK_FLAGS_RELEASE )
set(sqlite3_SQLite_SQLite3_LINKER_FLAGS_RELEASE
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${sqlite3_SQLite_SQLite3_SHARED_LINK_FLAGS_RELEASE}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${sqlite3_SQLite_SQLite3_SHARED_LINK_FLAGS_RELEASE}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${sqlite3_SQLite_SQLite3_EXE_LINK_FLAGS_RELEASE}>
)
list(APPEND sqlite3_BUILD_MODULES_PATHS_RELEASE )
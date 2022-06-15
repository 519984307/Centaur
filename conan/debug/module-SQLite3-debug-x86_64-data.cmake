########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

list(APPEND sqlite3_COMPONENT_NAMES SQLite::SQLite3)
list(REMOVE_DUPLICATES sqlite3_COMPONENT_NAMES)
set(sqlite3_FIND_DEPENDENCY_NAMES "")

########### VARIABLES #######################################################################
#############################################################################################
set(sqlite3_PACKAGE_FOLDER_DEBUG "/Users/ricardo/.conan/data/sqlite3/3.38.5/_/_/package/3d2c009873ddcd5176cbc0654fbdc8d5b85096ce")
set(sqlite3_INCLUDE_DIRS_DEBUG "${sqlite3_PACKAGE_FOLDER_DEBUG}/include")
set(sqlite3_RES_DIRS_DEBUG "${sqlite3_PACKAGE_FOLDER_DEBUG}/res")
set(sqlite3_DEFINITIONS_DEBUG )
set(sqlite3_SHARED_LINK_FLAGS_DEBUG )
set(sqlite3_EXE_LINK_FLAGS_DEBUG )
set(sqlite3_OBJECTS_DEBUG )
set(sqlite3_COMPILE_DEFINITIONS_DEBUG )
set(sqlite3_COMPILE_OPTIONS_C_DEBUG )
set(sqlite3_COMPILE_OPTIONS_CXX_DEBUG )
set(sqlite3_LIB_DIRS_DEBUG "${sqlite3_PACKAGE_FOLDER_DEBUG}/lib")
set(sqlite3_LIBS_DEBUG sqlite3)
set(sqlite3_SYSTEM_LIBS_DEBUG )
set(sqlite3_FRAMEWORK_DIRS_DEBUG "${sqlite3_PACKAGE_FOLDER_DEBUG}/Frameworks")
set(sqlite3_FRAMEWORKS_DEBUG )
set(sqlite3_BUILD_MODULES_PATHS_DEBUG )
set(sqlite3_BUILD_DIRS_DEBUG "${sqlite3_PACKAGE_FOLDER_DEBUG}/")

set(sqlite3_COMPONENTS_DEBUG SQLite::SQLite3)
########### COMPONENT SQLite::SQLite3 VARIABLES #############################################
set(sqlite3_SQLite_SQLite3_INCLUDE_DIRS_DEBUG "${sqlite3_PACKAGE_FOLDER_DEBUG}/include")
set(sqlite3_SQLite_SQLite3_LIB_DIRS_DEBUG "${sqlite3_PACKAGE_FOLDER_DEBUG}/lib")
set(sqlite3_SQLite_SQLite3_RES_DIRS_DEBUG "${sqlite3_PACKAGE_FOLDER_DEBUG}/res")
set(sqlite3_SQLite_SQLite3_DEFINITIONS_DEBUG )
set(sqlite3_SQLite_SQLite3_OBJECTS_DEBUG )
set(sqlite3_SQLite_SQLite3_COMPILE_DEFINITIONS_DEBUG )
set(sqlite3_SQLite_SQLite3_COMPILE_OPTIONS_C_DEBUG "")
set(sqlite3_SQLite_SQLite3_COMPILE_OPTIONS_CXX_DEBUG "")
set(sqlite3_SQLite_SQLite3_LIBS_DEBUG sqlite3)
set(sqlite3_SQLite_SQLite3_SYSTEM_LIBS_DEBUG )
set(sqlite3_SQLite_SQLite3_FRAMEWORK_DIRS_DEBUG "${sqlite3_PACKAGE_FOLDER_DEBUG}/Frameworks")
set(sqlite3_SQLite_SQLite3_FRAMEWORKS_DEBUG )
set(sqlite3_SQLite_SQLite3_DEPENDENCIES_DEBUG )
set(sqlite3_SQLite_SQLite3_SHARED_LINK_FLAGS_DEBUG )
set(sqlite3_SQLite_SQLite3_EXE_LINK_FLAGS_DEBUG )
set(sqlite3_SQLite_SQLite3_LINKER_FLAGS_DEBUG
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${sqlite3_SQLite_SQLite3_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${sqlite3_SQLite_SQLite3_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${sqlite3_SQLite_SQLite3_EXE_LINK_FLAGS_DEBUG}>
)
list(APPEND sqlite3_BUILD_MODULES_PATHS_DEBUG )
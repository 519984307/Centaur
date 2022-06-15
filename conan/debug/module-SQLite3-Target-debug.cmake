########### VARIABLES #######################################################################
#############################################################################################

set(sqlite3_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${sqlite3_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${sqlite3_COMPILE_OPTIONS_C_DEBUG}>")

set(sqlite3_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${sqlite3_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${sqlite3_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${sqlite3_EXE_LINK_FLAGS_DEBUG}>")

set(sqlite3_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(sqlite3_FRAMEWORKS_FOUND_DEBUG "${sqlite3_FRAMEWORKS_DEBUG}" "${sqlite3_FRAMEWORK_DIRS_DEBUG}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_sqlite3_DEPENDENCIES_DEBUG "${sqlite3_FRAMEWORKS_FOUND_DEBUG} ${sqlite3_SYSTEM_LIBS_DEBUG} ")

set(sqlite3_LIBRARIES_TARGETS_DEBUG "") # Will be filled later
set(sqlite3_LIBRARIES_DEBUG "") # Will be filled later
conan_package_library_targets("${sqlite3_LIBS_DEBUG}"    # libraries
                              "${sqlite3_LIB_DIRS_DEBUG}" # package_libdir
                              "${_sqlite3_DEPENDENCIES_DEBUG}" # deps
                              sqlite3_LIBRARIES_DEBUG   # out_libraries
                              sqlite3_LIBRARIES_TARGETS_DEBUG  # out_libraries_targets
                              "_DEBUG"  # config_suffix
                              "sqlite3")    # package_name

foreach(_FRAMEWORK ${sqlite3_FRAMEWORKS_FOUND_DEBUG})
    list(APPEND sqlite3_LIBRARIES_TARGETS_DEBUG ${_FRAMEWORK})
    list(APPEND sqlite3_LIBRARIES_DEBUG ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${sqlite3_SYSTEM_LIBS_DEBUG})
    list(APPEND sqlite3_LIBRARIES_TARGETS_DEBUG ${_SYSTEM_LIB})
    list(APPEND sqlite3_LIBRARIES_DEBUG ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(sqlite3_LIBRARIES_TARGETS_DEBUG "${sqlite3_LIBRARIES_TARGETS_DEBUG};")
set(sqlite3_LIBRARIES_DEBUG "${sqlite3_LIBRARIES_DEBUG};")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${sqlite3_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${sqlite3_BUILD_DIRS_DEBUG} ${CMAKE_PREFIX_PATH})
########## COMPONENT SQLite::SQLite3 FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(sqlite3_SQLite_SQLite3_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(sqlite3_SQLite_SQLite3_FRAMEWORKS_FOUND_DEBUG "${sqlite3_SQLite_SQLite3_FRAMEWORKS_DEBUG}" "${sqlite3_SQLite_SQLite3_FRAMEWORK_DIRS_DEBUG}")

set(sqlite3_SQLite_SQLite3_LIB_TARGETS_DEBUG "")
set(sqlite3_SQLite_SQLite3_NOT_USED_DEBUG "")
set(sqlite3_SQLite_SQLite3_LIBS_FRAMEWORKS_DEPS_DEBUG ${sqlite3_SQLite_SQLite3_FRAMEWORKS_FOUND_DEBUG} ${sqlite3_SQLite_SQLite3_SYSTEM_LIBS_DEBUG} ${sqlite3_SQLite_SQLite3_DEPENDENCIES_DEBUG})
conan_package_library_targets("${sqlite3_SQLite_SQLite3_LIBS_DEBUG}"
                              "${sqlite3_SQLite_SQLite3_LIB_DIRS_DEBUG}"
                              "${sqlite3_SQLite_SQLite3_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              sqlite3_SQLite_SQLite3_NOT_USED_DEBUG
                              sqlite3_SQLite_SQLite3_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "sqlite3_SQLite_SQLite3")

set(sqlite3_SQLite_SQLite3_LINK_LIBS_DEBUG ${sqlite3_SQLite_SQLite3_LIB_TARGETS_DEBUG} ${sqlite3_SQLite_SQLite3_LIBS_FRAMEWORKS_DEPS_DEBUG})


########## GLOBAL TARGET PROPERTIES Debug ########################################
set_property(TARGET SQLite::SQLite3
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${sqlite3_LIBRARIES_TARGETS_DEBUG}
                                           ${sqlite3_OBJECTS_DEBUG}> APPEND)
set_property(TARGET SQLite::SQLite3
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${sqlite3_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET SQLite::SQLite3
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${sqlite3_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET SQLite::SQLite3
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${sqlite3_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET SQLite::SQLite3
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:${sqlite3_COMPILE_OPTIONS_DEBUG}> APPEND)

########## COMPONENTS TARGET PROPERTIES Debug ########################################
########## COMPONENT SQLite::SQLite3 TARGET PROPERTIES ######################################
set_property(TARGET SQLite::SQLite3 PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${sqlite3_SQLite_SQLite3_LINK_LIBS_DEBUG}
             ${sqlite3_SQLite_SQLite3_OBJECTS_DEBUG}> APPEND)
set_property(TARGET SQLite::SQLite3 PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${sqlite3_SQLite_SQLite3_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET SQLite::SQLite3 PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${sqlite3_SQLite_SQLite3_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET SQLite::SQLite3 PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${sqlite3_SQLite_SQLite3_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET SQLite::SQLite3 PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${sqlite3_SQLite_SQLite3_COMPILE_OPTIONS_C_DEBUG}
             ${sqlite3_SQLite_SQLite3_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(sqlite3_SQLite_SQLite3_TARGET_PROPERTIES TRUE)
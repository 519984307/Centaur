########### VARIABLES #######################################################################
#############################################################################################

set(sqlite3_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${sqlite3_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${sqlite3_COMPILE_OPTIONS_C_RELEASE}>")

set(sqlite3_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${sqlite3_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${sqlite3_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${sqlite3_EXE_LINK_FLAGS_RELEASE}>")

set(sqlite3_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(sqlite3_FRAMEWORKS_FOUND_RELEASE "${sqlite3_FRAMEWORKS_RELEASE}" "${sqlite3_FRAMEWORK_DIRS_RELEASE}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_sqlite3_DEPENDENCIES_RELEASE "${sqlite3_FRAMEWORKS_FOUND_RELEASE} ${sqlite3_SYSTEM_LIBS_RELEASE} ")

set(sqlite3_LIBRARIES_TARGETS_RELEASE "") # Will be filled later
set(sqlite3_LIBRARIES_RELEASE "") # Will be filled later
conan_package_library_targets("${sqlite3_LIBS_RELEASE}"    # libraries
                              "${sqlite3_LIB_DIRS_RELEASE}" # package_libdir
                              "${_sqlite3_DEPENDENCIES_RELEASE}" # deps
                              sqlite3_LIBRARIES_RELEASE   # out_libraries
                              sqlite3_LIBRARIES_TARGETS_RELEASE  # out_libraries_targets
                              "_RELEASE"  # config_suffix
                              "sqlite3")    # package_name

foreach(_FRAMEWORK ${sqlite3_FRAMEWORKS_FOUND_RELEASE})
    list(APPEND sqlite3_LIBRARIES_TARGETS_RELEASE ${_FRAMEWORK})
    list(APPEND sqlite3_LIBRARIES_RELEASE ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${sqlite3_SYSTEM_LIBS_RELEASE})
    list(APPEND sqlite3_LIBRARIES_TARGETS_RELEASE ${_SYSTEM_LIB})
    list(APPEND sqlite3_LIBRARIES_RELEASE ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(sqlite3_LIBRARIES_TARGETS_RELEASE "${sqlite3_LIBRARIES_TARGETS_RELEASE};")
set(sqlite3_LIBRARIES_RELEASE "${sqlite3_LIBRARIES_RELEASE};")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${sqlite3_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${sqlite3_BUILD_DIRS_RELEASE} ${CMAKE_PREFIX_PATH})
########## COMPONENT SQLite::SQLite3 FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(sqlite3_SQLite_SQLite3_FRAMEWORKS_FOUND_RELEASE "")
conan_find_apple_frameworks(sqlite3_SQLite_SQLite3_FRAMEWORKS_FOUND_RELEASE "${sqlite3_SQLite_SQLite3_FRAMEWORKS_RELEASE}" "${sqlite3_SQLite_SQLite3_FRAMEWORK_DIRS_RELEASE}")

set(sqlite3_SQLite_SQLite3_LIB_TARGETS_RELEASE "")
set(sqlite3_SQLite_SQLite3_NOT_USED_RELEASE "")
set(sqlite3_SQLite_SQLite3_LIBS_FRAMEWORKS_DEPS_RELEASE ${sqlite3_SQLite_SQLite3_FRAMEWORKS_FOUND_RELEASE} ${sqlite3_SQLite_SQLite3_SYSTEM_LIBS_RELEASE} ${sqlite3_SQLite_SQLite3_DEPENDENCIES_RELEASE})
conan_package_library_targets("${sqlite3_SQLite_SQLite3_LIBS_RELEASE}"
                              "${sqlite3_SQLite_SQLite3_LIB_DIRS_RELEASE}"
                              "${sqlite3_SQLite_SQLite3_LIBS_FRAMEWORKS_DEPS_RELEASE}"
                              sqlite3_SQLite_SQLite3_NOT_USED_RELEASE
                              sqlite3_SQLite_SQLite3_LIB_TARGETS_RELEASE
                              "_RELEASE"
                              "sqlite3_SQLite_SQLite3")

set(sqlite3_SQLite_SQLite3_LINK_LIBS_RELEASE ${sqlite3_SQLite_SQLite3_LIB_TARGETS_RELEASE} ${sqlite3_SQLite_SQLite3_LIBS_FRAMEWORKS_DEPS_RELEASE})


########## GLOBAL TARGET PROPERTIES Release ########################################
set_property(TARGET SQLite::SQLite3
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${sqlite3_LIBRARIES_TARGETS_RELEASE}
                                           ${sqlite3_OBJECTS_RELEASE}> APPEND)
set_property(TARGET SQLite::SQLite3
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Release>:${sqlite3_LINKER_FLAGS_RELEASE}> APPEND)
set_property(TARGET SQLite::SQLite3
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Release>:${sqlite3_INCLUDE_DIRS_RELEASE}> APPEND)
set_property(TARGET SQLite::SQLite3
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Release>:${sqlite3_COMPILE_DEFINITIONS_RELEASE}> APPEND)
set_property(TARGET SQLite::SQLite3
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Release>:${sqlite3_COMPILE_OPTIONS_RELEASE}> APPEND)

########## COMPONENTS TARGET PROPERTIES Release ########################################
########## COMPONENT SQLite::SQLite3 TARGET PROPERTIES ######################################
set_property(TARGET SQLite::SQLite3 PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${sqlite3_SQLite_SQLite3_LINK_LIBS_RELEASE}
             ${sqlite3_SQLite_SQLite3_OBJECTS_RELEASE}> APPEND)
set_property(TARGET SQLite::SQLite3 PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Release>:${sqlite3_SQLite_SQLite3_LINKER_FLAGS_RELEASE}> APPEND)
set_property(TARGET SQLite::SQLite3 PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Release>:${sqlite3_SQLite_SQLite3_INCLUDE_DIRS_RELEASE}> APPEND)
set_property(TARGET SQLite::SQLite3 PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Release>:${sqlite3_SQLite_SQLite3_COMPILE_DEFINITIONS_RELEASE}> APPEND)
set_property(TARGET SQLite::SQLite3 PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Release>:
             ${sqlite3_SQLite_SQLite3_COMPILE_OPTIONS_C_RELEASE}
             ${sqlite3_SQLite_SQLite3_COMPILE_OPTIONS_CXX_RELEASE}> APPEND)
set(sqlite3_SQLite_SQLite3_TARGET_PROPERTIES TRUE)
########### VARIABLES #######################################################################
#############################################################################################

set(yaml-cpp_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${yaml-cpp_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${yaml-cpp_COMPILE_OPTIONS_C_RELEASE}>")

set(yaml-cpp_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${yaml-cpp_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${yaml-cpp_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${yaml-cpp_EXE_LINK_FLAGS_RELEASE}>")

set(yaml-cpp_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(yaml-cpp_FRAMEWORKS_FOUND_RELEASE "${yaml-cpp_FRAMEWORKS_RELEASE}" "${yaml-cpp_FRAMEWORK_DIRS_RELEASE}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_yaml-cpp_DEPENDENCIES_RELEASE "${yaml-cpp_FRAMEWORKS_FOUND_RELEASE} ${yaml-cpp_SYSTEM_LIBS_RELEASE} ")

set(yaml-cpp_LIBRARIES_TARGETS_RELEASE "") # Will be filled later
set(yaml-cpp_LIBRARIES_RELEASE "") # Will be filled later
conan_package_library_targets("${yaml-cpp_LIBS_RELEASE}"    # libraries
                              "${yaml-cpp_LIB_DIRS_RELEASE}" # package_libdir
                              "${_yaml-cpp_DEPENDENCIES_RELEASE}" # deps
                              yaml-cpp_LIBRARIES_RELEASE   # out_libraries
                              yaml-cpp_LIBRARIES_TARGETS_RELEASE  # out_libraries_targets
                              "_RELEASE"  # config_suffix
                              "yaml-cpp")    # package_name

foreach(_FRAMEWORK ${yaml-cpp_FRAMEWORKS_FOUND_RELEASE})
    list(APPEND yaml-cpp_LIBRARIES_TARGETS_RELEASE ${_FRAMEWORK})
    list(APPEND yaml-cpp_LIBRARIES_RELEASE ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${yaml-cpp_SYSTEM_LIBS_RELEASE})
    list(APPEND yaml-cpp_LIBRARIES_TARGETS_RELEASE ${_SYSTEM_LIB})
    list(APPEND yaml-cpp_LIBRARIES_RELEASE ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(yaml-cpp_LIBRARIES_TARGETS_RELEASE "${yaml-cpp_LIBRARIES_TARGETS_RELEASE};")
set(yaml-cpp_LIBRARIES_RELEASE "${yaml-cpp_LIBRARIES_RELEASE};")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${yaml-cpp_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${yaml-cpp_BUILD_DIRS_RELEASE} ${CMAKE_PREFIX_PATH})


########## GLOBAL TARGET PROPERTIES Release ########################################
set_property(TARGET yaml-cpp
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${yaml-cpp_LIBRARIES_TARGETS_RELEASE}
                                           ${yaml-cpp_OBJECTS_RELEASE}> APPEND)
set_property(TARGET yaml-cpp
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Release>:${yaml-cpp_LINKER_FLAGS_RELEASE}> APPEND)
set_property(TARGET yaml-cpp
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Release>:${yaml-cpp_INCLUDE_DIRS_RELEASE}> APPEND)
set_property(TARGET yaml-cpp
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Release>:${yaml-cpp_COMPILE_DEFINITIONS_RELEASE}> APPEND)
set_property(TARGET yaml-cpp
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Release>:${yaml-cpp_COMPILE_OPTIONS_RELEASE}> APPEND)

########## COMPONENTS TARGET PROPERTIES Release ########################################
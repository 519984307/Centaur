########### VARIABLES #######################################################################
#############################################################################################

set(xerces-c_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${xerces-c_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${xerces-c_COMPILE_OPTIONS_C_RELEASE}>")

set(xerces-c_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${xerces-c_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${xerces-c_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${xerces-c_EXE_LINK_FLAGS_RELEASE}>")

set(xerces-c_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(xerces-c_FRAMEWORKS_FOUND_RELEASE "${xerces-c_FRAMEWORKS_RELEASE}" "${xerces-c_FRAMEWORK_DIRS_RELEASE}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_xerces-c_DEPENDENCIES_RELEASE "${xerces-c_FRAMEWORKS_FOUND_RELEASE} ${xerces-c_SYSTEM_LIBS_RELEASE} ")

set(xerces-c_LIBRARIES_TARGETS_RELEASE "") # Will be filled later
set(xerces-c_LIBRARIES_RELEASE "") # Will be filled later
conan_package_library_targets("${xerces-c_LIBS_RELEASE}"    # libraries
                              "${xerces-c_LIB_DIRS_RELEASE}" # package_libdir
                              "${_xerces-c_DEPENDENCIES_RELEASE}" # deps
                              xerces-c_LIBRARIES_RELEASE   # out_libraries
                              xerces-c_LIBRARIES_TARGETS_RELEASE  # out_libraries_targets
                              "_RELEASE"  # config_suffix
                              "xerces-c")    # package_name

foreach(_FRAMEWORK ${xerces-c_FRAMEWORKS_FOUND_RELEASE})
    list(APPEND xerces-c_LIBRARIES_TARGETS_RELEASE ${_FRAMEWORK})
    list(APPEND xerces-c_LIBRARIES_RELEASE ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${xerces-c_SYSTEM_LIBS_RELEASE})
    list(APPEND xerces-c_LIBRARIES_TARGETS_RELEASE ${_SYSTEM_LIB})
    list(APPEND xerces-c_LIBRARIES_RELEASE ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(xerces-c_LIBRARIES_TARGETS_RELEASE "${xerces-c_LIBRARIES_TARGETS_RELEASE};")
set(xerces-c_LIBRARIES_RELEASE "${xerces-c_LIBRARIES_RELEASE};")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${xerces-c_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${xerces-c_BUILD_DIRS_RELEASE} ${CMAKE_PREFIX_PATH})


########## GLOBAL TARGET PROPERTIES Release ########################################
set_property(TARGET XercesC::XercesC
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${xerces-c_LIBRARIES_TARGETS_RELEASE}
                                           ${xerces-c_OBJECTS_RELEASE}> APPEND)
set_property(TARGET XercesC::XercesC
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Release>:${xerces-c_LINKER_FLAGS_RELEASE}> APPEND)
set_property(TARGET XercesC::XercesC
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Release>:${xerces-c_INCLUDE_DIRS_RELEASE}> APPEND)
set_property(TARGET XercesC::XercesC
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Release>:${xerces-c_COMPILE_DEFINITIONS_RELEASE}> APPEND)
set_property(TARGET XercesC::XercesC
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Release>:${xerces-c_COMPILE_OPTIONS_RELEASE}> APPEND)

########## COMPONENTS TARGET PROPERTIES Release ########################################
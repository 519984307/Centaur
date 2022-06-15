########### VARIABLES #######################################################################
#############################################################################################

set(xerces-c_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${xerces-c_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${xerces-c_COMPILE_OPTIONS_C_DEBUG}>")

set(xerces-c_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${xerces-c_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${xerces-c_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${xerces-c_EXE_LINK_FLAGS_DEBUG}>")

set(xerces-c_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(xerces-c_FRAMEWORKS_FOUND_DEBUG "${xerces-c_FRAMEWORKS_DEBUG}" "${xerces-c_FRAMEWORK_DIRS_DEBUG}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_xerces-c_DEPENDENCIES_DEBUG "${xerces-c_FRAMEWORKS_FOUND_DEBUG} ${xerces-c_SYSTEM_LIBS_DEBUG} ")

set(xerces-c_LIBRARIES_TARGETS_DEBUG "") # Will be filled later
set(xerces-c_LIBRARIES_DEBUG "") # Will be filled later
conan_package_library_targets("${xerces-c_LIBS_DEBUG}"    # libraries
                              "${xerces-c_LIB_DIRS_DEBUG}" # package_libdir
                              "${_xerces-c_DEPENDENCIES_DEBUG}" # deps
                              xerces-c_LIBRARIES_DEBUG   # out_libraries
                              xerces-c_LIBRARIES_TARGETS_DEBUG  # out_libraries_targets
                              "_DEBUG"  # config_suffix
                              "xerces-c")    # package_name

foreach(_FRAMEWORK ${xerces-c_FRAMEWORKS_FOUND_DEBUG})
    list(APPEND xerces-c_LIBRARIES_TARGETS_DEBUG ${_FRAMEWORK})
    list(APPEND xerces-c_LIBRARIES_DEBUG ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${xerces-c_SYSTEM_LIBS_DEBUG})
    list(APPEND xerces-c_LIBRARIES_TARGETS_DEBUG ${_SYSTEM_LIB})
    list(APPEND xerces-c_LIBRARIES_DEBUG ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(xerces-c_LIBRARIES_TARGETS_DEBUG "${xerces-c_LIBRARIES_TARGETS_DEBUG};")
set(xerces-c_LIBRARIES_DEBUG "${xerces-c_LIBRARIES_DEBUG};")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${xerces-c_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${xerces-c_BUILD_DIRS_DEBUG} ${CMAKE_PREFIX_PATH})


########## GLOBAL TARGET PROPERTIES Debug ########################################
set_property(TARGET XercesC::XercesC
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${xerces-c_LIBRARIES_TARGETS_DEBUG}
                                           ${xerces-c_OBJECTS_DEBUG}> APPEND)
set_property(TARGET XercesC::XercesC
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${xerces-c_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET XercesC::XercesC
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${xerces-c_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET XercesC::XercesC
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${xerces-c_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET XercesC::XercesC
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:${xerces-c_COMPILE_OPTIONS_DEBUG}> APPEND)

########## COMPONENTS TARGET PROPERTIES Debug ########################################
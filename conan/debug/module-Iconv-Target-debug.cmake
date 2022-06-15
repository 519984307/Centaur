########### VARIABLES #######################################################################
#############################################################################################

set(libiconv_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${libiconv_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${libiconv_COMPILE_OPTIONS_C_DEBUG}>")

set(libiconv_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${libiconv_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${libiconv_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${libiconv_EXE_LINK_FLAGS_DEBUG}>")

set(libiconv_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(libiconv_FRAMEWORKS_FOUND_DEBUG "${libiconv_FRAMEWORKS_DEBUG}" "${libiconv_FRAMEWORK_DIRS_DEBUG}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_libiconv_DEPENDENCIES_DEBUG "${libiconv_FRAMEWORKS_FOUND_DEBUG} ${libiconv_SYSTEM_LIBS_DEBUG} ")

set(libiconv_LIBRARIES_TARGETS_DEBUG "") # Will be filled later
set(libiconv_LIBRARIES_DEBUG "") # Will be filled later
conan_package_library_targets("${libiconv_LIBS_DEBUG}"    # libraries
                              "${libiconv_LIB_DIRS_DEBUG}" # package_libdir
                              "${_libiconv_DEPENDENCIES_DEBUG}" # deps
                              libiconv_LIBRARIES_DEBUG   # out_libraries
                              libiconv_LIBRARIES_TARGETS_DEBUG  # out_libraries_targets
                              "_DEBUG"  # config_suffix
                              "libiconv")    # package_name

foreach(_FRAMEWORK ${libiconv_FRAMEWORKS_FOUND_DEBUG})
    list(APPEND libiconv_LIBRARIES_TARGETS_DEBUG ${_FRAMEWORK})
    list(APPEND libiconv_LIBRARIES_DEBUG ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${libiconv_SYSTEM_LIBS_DEBUG})
    list(APPEND libiconv_LIBRARIES_TARGETS_DEBUG ${_SYSTEM_LIB})
    list(APPEND libiconv_LIBRARIES_DEBUG ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(libiconv_LIBRARIES_TARGETS_DEBUG "${libiconv_LIBRARIES_TARGETS_DEBUG};")
set(libiconv_LIBRARIES_DEBUG "${libiconv_LIBRARIES_DEBUG};")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${libiconv_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${libiconv_BUILD_DIRS_DEBUG} ${CMAKE_PREFIX_PATH})


########## GLOBAL TARGET PROPERTIES Debug ########################################
set_property(TARGET Iconv::Iconv
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${libiconv_LIBRARIES_TARGETS_DEBUG}
                                           ${libiconv_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Iconv::Iconv
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${libiconv_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Iconv::Iconv
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${libiconv_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Iconv::Iconv
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${libiconv_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Iconv::Iconv
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:${libiconv_COMPILE_OPTIONS_DEBUG}> APPEND)

########## COMPONENTS TARGET PROPERTIES Debug ########################################
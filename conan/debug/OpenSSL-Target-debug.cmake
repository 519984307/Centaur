########### VARIABLES #######################################################################
#############################################################################################

set(openssl_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${openssl_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${openssl_COMPILE_OPTIONS_C_DEBUG}>")

set(openssl_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${openssl_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${openssl_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${openssl_EXE_LINK_FLAGS_DEBUG}>")

set(openssl_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(openssl_FRAMEWORKS_FOUND_DEBUG "${openssl_FRAMEWORKS_DEBUG}" "${openssl_FRAMEWORK_DIRS_DEBUG}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_openssl_DEPENDENCIES_DEBUG "${openssl_FRAMEWORKS_FOUND_DEBUG} ${openssl_SYSTEM_LIBS_DEBUG} OpenSSL::Crypto")

set(openssl_LIBRARIES_TARGETS_DEBUG "") # Will be filled later
set(openssl_LIBRARIES_DEBUG "") # Will be filled later
conan_package_library_targets("${openssl_LIBS_DEBUG}"    # libraries
                              "${openssl_LIB_DIRS_DEBUG}" # package_libdir
                              "${_openssl_DEPENDENCIES_DEBUG}" # deps
                              openssl_LIBRARIES_DEBUG   # out_libraries
                              openssl_LIBRARIES_TARGETS_DEBUG  # out_libraries_targets
                              "_DEBUG"  # config_suffix
                              "openssl")    # package_name

foreach(_FRAMEWORK ${openssl_FRAMEWORKS_FOUND_DEBUG})
    list(APPEND openssl_LIBRARIES_TARGETS_DEBUG ${_FRAMEWORK})
    list(APPEND openssl_LIBRARIES_DEBUG ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${openssl_SYSTEM_LIBS_DEBUG})
    list(APPEND openssl_LIBRARIES_TARGETS_DEBUG ${_SYSTEM_LIB})
    list(APPEND openssl_LIBRARIES_DEBUG ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(openssl_LIBRARIES_TARGETS_DEBUG "${openssl_LIBRARIES_TARGETS_DEBUG};OpenSSL::Crypto")
set(openssl_LIBRARIES_DEBUG "${openssl_LIBRARIES_DEBUG};OpenSSL::Crypto")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${openssl_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${openssl_BUILD_DIRS_DEBUG} ${CMAKE_PREFIX_PATH})
########## COMPONENT OpenSSL::SSL FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(openssl_OpenSSL_SSL_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(openssl_OpenSSL_SSL_FRAMEWORKS_FOUND_DEBUG "${openssl_OpenSSL_SSL_FRAMEWORKS_DEBUG}" "${openssl_OpenSSL_SSL_FRAMEWORK_DIRS_DEBUG}")

set(openssl_OpenSSL_SSL_LIB_TARGETS_DEBUG "")
set(openssl_OpenSSL_SSL_NOT_USED_DEBUG "")
set(openssl_OpenSSL_SSL_LIBS_FRAMEWORKS_DEPS_DEBUG ${openssl_OpenSSL_SSL_FRAMEWORKS_FOUND_DEBUG} ${openssl_OpenSSL_SSL_SYSTEM_LIBS_DEBUG} ${openssl_OpenSSL_SSL_DEPENDENCIES_DEBUG})
conan_package_library_targets("${openssl_OpenSSL_SSL_LIBS_DEBUG}"
                              "${openssl_OpenSSL_SSL_LIB_DIRS_DEBUG}"
                              "${openssl_OpenSSL_SSL_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              openssl_OpenSSL_SSL_NOT_USED_DEBUG
                              openssl_OpenSSL_SSL_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "openssl_OpenSSL_SSL")

set(openssl_OpenSSL_SSL_LINK_LIBS_DEBUG ${openssl_OpenSSL_SSL_LIB_TARGETS_DEBUG} ${openssl_OpenSSL_SSL_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT OpenSSL::Crypto FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(openssl_OpenSSL_Crypto_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(openssl_OpenSSL_Crypto_FRAMEWORKS_FOUND_DEBUG "${openssl_OpenSSL_Crypto_FRAMEWORKS_DEBUG}" "${openssl_OpenSSL_Crypto_FRAMEWORK_DIRS_DEBUG}")

set(openssl_OpenSSL_Crypto_LIB_TARGETS_DEBUG "")
set(openssl_OpenSSL_Crypto_NOT_USED_DEBUG "")
set(openssl_OpenSSL_Crypto_LIBS_FRAMEWORKS_DEPS_DEBUG ${openssl_OpenSSL_Crypto_FRAMEWORKS_FOUND_DEBUG} ${openssl_OpenSSL_Crypto_SYSTEM_LIBS_DEBUG} ${openssl_OpenSSL_Crypto_DEPENDENCIES_DEBUG})
conan_package_library_targets("${openssl_OpenSSL_Crypto_LIBS_DEBUG}"
                              "${openssl_OpenSSL_Crypto_LIB_DIRS_DEBUG}"
                              "${openssl_OpenSSL_Crypto_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              openssl_OpenSSL_Crypto_NOT_USED_DEBUG
                              openssl_OpenSSL_Crypto_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "openssl_OpenSSL_Crypto")

set(openssl_OpenSSL_Crypto_LINK_LIBS_DEBUG ${openssl_OpenSSL_Crypto_LIB_TARGETS_DEBUG} ${openssl_OpenSSL_Crypto_LIBS_FRAMEWORKS_DEPS_DEBUG})


########## GLOBAL TARGET PROPERTIES Debug ########################################
set_property(TARGET openssl::openssl
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${openssl_LIBRARIES_TARGETS_DEBUG}
                                           ${openssl_OBJECTS_DEBUG}> APPEND)
set_property(TARGET openssl::openssl
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${openssl_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET openssl::openssl
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${openssl_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET openssl::openssl
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${openssl_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET openssl::openssl
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:${openssl_COMPILE_OPTIONS_DEBUG}> APPEND)

########## COMPONENTS TARGET PROPERTIES Debug ########################################
########## COMPONENT OpenSSL::SSL TARGET PROPERTIES ######################################
set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${openssl_OpenSSL_SSL_LINK_LIBS_DEBUG}
             ${openssl_OpenSSL_SSL_OBJECTS_DEBUG}> APPEND)
set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${openssl_OpenSSL_SSL_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${openssl_OpenSSL_SSL_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${openssl_OpenSSL_SSL_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${openssl_OpenSSL_SSL_COMPILE_OPTIONS_C_DEBUG}
             ${openssl_OpenSSL_SSL_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(openssl_OpenSSL_SSL_TARGET_PROPERTIES TRUE)
########## COMPONENT OpenSSL::Crypto TARGET PROPERTIES ######################################
set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${openssl_OpenSSL_Crypto_LINK_LIBS_DEBUG}
             ${openssl_OpenSSL_Crypto_OBJECTS_DEBUG}> APPEND)
set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${openssl_OpenSSL_Crypto_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${openssl_OpenSSL_Crypto_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${openssl_OpenSSL_Crypto_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${openssl_OpenSSL_Crypto_COMPILE_OPTIONS_C_DEBUG}
             ${openssl_OpenSSL_Crypto_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(openssl_OpenSSL_Crypto_TARGET_PROPERTIES TRUE)
########### VARIABLES #######################################################################
#############################################################################################

set(openssl_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${openssl_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${openssl_COMPILE_OPTIONS_C_RELEASE}>")

set(openssl_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${openssl_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${openssl_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${openssl_EXE_LINK_FLAGS_RELEASE}>")

set(openssl_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(openssl_FRAMEWORKS_FOUND_RELEASE "${openssl_FRAMEWORKS_RELEASE}" "${openssl_FRAMEWORK_DIRS_RELEASE}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_openssl_DEPENDENCIES_RELEASE "${openssl_FRAMEWORKS_FOUND_RELEASE} ${openssl_SYSTEM_LIBS_RELEASE} OpenSSL::Crypto")

set(openssl_LIBRARIES_TARGETS_RELEASE "") # Will be filled later
set(openssl_LIBRARIES_RELEASE "") # Will be filled later
conan_package_library_targets("${openssl_LIBS_RELEASE}"    # libraries
                              "${openssl_LIB_DIRS_RELEASE}" # package_libdir
                              "${_openssl_DEPENDENCIES_RELEASE}" # deps
                              openssl_LIBRARIES_RELEASE   # out_libraries
                              openssl_LIBRARIES_TARGETS_RELEASE  # out_libraries_targets
                              "_RELEASE"  # config_suffix
                              "openssl")    # package_name

foreach(_FRAMEWORK ${openssl_FRAMEWORKS_FOUND_RELEASE})
    list(APPEND openssl_LIBRARIES_TARGETS_RELEASE ${_FRAMEWORK})
    list(APPEND openssl_LIBRARIES_RELEASE ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${openssl_SYSTEM_LIBS_RELEASE})
    list(APPEND openssl_LIBRARIES_TARGETS_RELEASE ${_SYSTEM_LIB})
    list(APPEND openssl_LIBRARIES_RELEASE ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(openssl_LIBRARIES_TARGETS_RELEASE "${openssl_LIBRARIES_TARGETS_RELEASE};OpenSSL::Crypto")
set(openssl_LIBRARIES_RELEASE "${openssl_LIBRARIES_RELEASE};OpenSSL::Crypto")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${openssl_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${openssl_BUILD_DIRS_RELEASE} ${CMAKE_PREFIX_PATH})
########## COMPONENT OpenSSL::SSL FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(openssl_OpenSSL_SSL_FRAMEWORKS_FOUND_RELEASE "")
conan_find_apple_frameworks(openssl_OpenSSL_SSL_FRAMEWORKS_FOUND_RELEASE "${openssl_OpenSSL_SSL_FRAMEWORKS_RELEASE}" "${openssl_OpenSSL_SSL_FRAMEWORK_DIRS_RELEASE}")

set(openssl_OpenSSL_SSL_LIB_TARGETS_RELEASE "")
set(openssl_OpenSSL_SSL_NOT_USED_RELEASE "")
set(openssl_OpenSSL_SSL_LIBS_FRAMEWORKS_DEPS_RELEASE ${openssl_OpenSSL_SSL_FRAMEWORKS_FOUND_RELEASE} ${openssl_OpenSSL_SSL_SYSTEM_LIBS_RELEASE} ${openssl_OpenSSL_SSL_DEPENDENCIES_RELEASE})
conan_package_library_targets("${openssl_OpenSSL_SSL_LIBS_RELEASE}"
                              "${openssl_OpenSSL_SSL_LIB_DIRS_RELEASE}"
                              "${openssl_OpenSSL_SSL_LIBS_FRAMEWORKS_DEPS_RELEASE}"
                              openssl_OpenSSL_SSL_NOT_USED_RELEASE
                              openssl_OpenSSL_SSL_LIB_TARGETS_RELEASE
                              "_RELEASE"
                              "openssl_OpenSSL_SSL")

set(openssl_OpenSSL_SSL_LINK_LIBS_RELEASE ${openssl_OpenSSL_SSL_LIB_TARGETS_RELEASE} ${openssl_OpenSSL_SSL_LIBS_FRAMEWORKS_DEPS_RELEASE})
########## COMPONENT OpenSSL::Crypto FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(openssl_OpenSSL_Crypto_FRAMEWORKS_FOUND_RELEASE "")
conan_find_apple_frameworks(openssl_OpenSSL_Crypto_FRAMEWORKS_FOUND_RELEASE "${openssl_OpenSSL_Crypto_FRAMEWORKS_RELEASE}" "${openssl_OpenSSL_Crypto_FRAMEWORK_DIRS_RELEASE}")

set(openssl_OpenSSL_Crypto_LIB_TARGETS_RELEASE "")
set(openssl_OpenSSL_Crypto_NOT_USED_RELEASE "")
set(openssl_OpenSSL_Crypto_LIBS_FRAMEWORKS_DEPS_RELEASE ${openssl_OpenSSL_Crypto_FRAMEWORKS_FOUND_RELEASE} ${openssl_OpenSSL_Crypto_SYSTEM_LIBS_RELEASE} ${openssl_OpenSSL_Crypto_DEPENDENCIES_RELEASE})
conan_package_library_targets("${openssl_OpenSSL_Crypto_LIBS_RELEASE}"
                              "${openssl_OpenSSL_Crypto_LIB_DIRS_RELEASE}"
                              "${openssl_OpenSSL_Crypto_LIBS_FRAMEWORKS_DEPS_RELEASE}"
                              openssl_OpenSSL_Crypto_NOT_USED_RELEASE
                              openssl_OpenSSL_Crypto_LIB_TARGETS_RELEASE
                              "_RELEASE"
                              "openssl_OpenSSL_Crypto")

set(openssl_OpenSSL_Crypto_LINK_LIBS_RELEASE ${openssl_OpenSSL_Crypto_LIB_TARGETS_RELEASE} ${openssl_OpenSSL_Crypto_LIBS_FRAMEWORKS_DEPS_RELEASE})


########## GLOBAL TARGET PROPERTIES Release ########################################
set_property(TARGET openssl::openssl
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${openssl_LIBRARIES_TARGETS_RELEASE}
                                           ${openssl_OBJECTS_RELEASE}> APPEND)
set_property(TARGET openssl::openssl
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Release>:${openssl_LINKER_FLAGS_RELEASE}> APPEND)
set_property(TARGET openssl::openssl
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Release>:${openssl_INCLUDE_DIRS_RELEASE}> APPEND)
set_property(TARGET openssl::openssl
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Release>:${openssl_COMPILE_DEFINITIONS_RELEASE}> APPEND)
set_property(TARGET openssl::openssl
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Release>:${openssl_COMPILE_OPTIONS_RELEASE}> APPEND)

########## COMPONENTS TARGET PROPERTIES Release ########################################
########## COMPONENT OpenSSL::SSL TARGET PROPERTIES ######################################
set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${openssl_OpenSSL_SSL_LINK_LIBS_RELEASE}
             ${openssl_OpenSSL_SSL_OBJECTS_RELEASE}> APPEND)
set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Release>:${openssl_OpenSSL_SSL_LINKER_FLAGS_RELEASE}> APPEND)
set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Release>:${openssl_OpenSSL_SSL_INCLUDE_DIRS_RELEASE}> APPEND)
set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Release>:${openssl_OpenSSL_SSL_COMPILE_DEFINITIONS_RELEASE}> APPEND)
set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Release>:
             ${openssl_OpenSSL_SSL_COMPILE_OPTIONS_C_RELEASE}
             ${openssl_OpenSSL_SSL_COMPILE_OPTIONS_CXX_RELEASE}> APPEND)
set(openssl_OpenSSL_SSL_TARGET_PROPERTIES TRUE)
########## COMPONENT OpenSSL::Crypto TARGET PROPERTIES ######################################
set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${openssl_OpenSSL_Crypto_LINK_LIBS_RELEASE}
             ${openssl_OpenSSL_Crypto_OBJECTS_RELEASE}> APPEND)
set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Release>:${openssl_OpenSSL_Crypto_LINKER_FLAGS_RELEASE}> APPEND)
set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Release>:${openssl_OpenSSL_Crypto_INCLUDE_DIRS_RELEASE}> APPEND)
set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Release>:${openssl_OpenSSL_Crypto_COMPILE_DEFINITIONS_RELEASE}> APPEND)
set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Release>:
             ${openssl_OpenSSL_Crypto_COMPILE_OPTIONS_C_RELEASE}
             ${openssl_OpenSSL_Crypto_COMPILE_OPTIONS_CXX_RELEASE}> APPEND)
set(openssl_OpenSSL_Crypto_TARGET_PROPERTIES TRUE)
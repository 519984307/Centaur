########### VARIABLES #######################################################################
#############################################################################################

set(boost_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${boost_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${boost_COMPILE_OPTIONS_C_DEBUG}>")

set(boost_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${boost_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${boost_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${boost_EXE_LINK_FLAGS_DEBUG}>")

set(boost_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(boost_FRAMEWORKS_FOUND_DEBUG "${boost_FRAMEWORKS_DEBUG}" "${boost_FRAMEWORK_DIRS_DEBUG}")

# Gather all the libraries that should be linked to the targets (do not touch existing variables)
set(_boost_DEPENDENCIES_DEBUG "${boost_FRAMEWORKS_FOUND_DEBUG} ${boost_SYSTEM_LIBS_DEBUG} Boost::headers;Boost::headers;Boost::diagnostic_definitions;Boost::disable_autolinking;Boost::dynamic_linking;boost::_libboost;Boost::system;boost::_libboost;boost::_libboost;boost::_libboost;Boost::exception;Boost::thread;boost::_libboost;Boost::context;Boost::exception;Boost::system;boost::_libboost;boost::_libboost;boost::_libboost;Boost::atomic;Boost::system;boost::_libboost;BZip2::BZip2;ZLIB::ZLIB;Boost::random;Boost::regex;boost::_libboost;Iconv::Iconv;Boost::thread;boost::_libboost;Boost::atomic;Boost::container;Boost::date_time;Boost::exception;Boost::filesystem;Boost::random;Boost::regex;Boost::system;Boost::thread;boost::_libboost;Boost::log;boost::_libboost;Boost::test;boost::_libboost;boost::_libboost;Boost::system;boost::_libboost;boost::_libboost;boost::_libboost;boost::_libboost;Boost::stacktrace;boost::_libboost;libbacktrace::libbacktrace;Boost::stacktrace;boost::_libboost;Boost::stacktrace;boost::_libboost;Boost::stacktrace;boost::_libboost;boost::_libboost;Boost::exception;boost::_libboost;Boost::test;boost::_libboost;Boost::atomic;Boost::chrono;Boost::container;Boost::date_time;Boost::exception;Boost::system;boost::_libboost;Boost::chrono;Boost::system;boost::_libboost;Boost::thread;boost::_libboost;Boost::prg_exec_monitor;Boost::test;Boost::test_exec_monitor;boost::_libboost;Boost::filesystem;Boost::serialization;boost::_libboost;Boost::serialization;boost::_libboost")

set(boost_LIBRARIES_TARGETS_DEBUG "") # Will be filled later
set(boost_LIBRARIES_DEBUG "") # Will be filled later
conan_package_library_targets("${boost_LIBS_DEBUG}"    # libraries
                              "${boost_LIB_DIRS_DEBUG}" # package_libdir
                              "${_boost_DEPENDENCIES_DEBUG}" # deps
                              boost_LIBRARIES_DEBUG   # out_libraries
                              boost_LIBRARIES_TARGETS_DEBUG  # out_libraries_targets
                              "_DEBUG"  # config_suffix
                              "boost")    # package_name

foreach(_FRAMEWORK ${boost_FRAMEWORKS_FOUND_DEBUG})
    list(APPEND boost_LIBRARIES_TARGETS_DEBUG ${_FRAMEWORK})
    list(APPEND boost_LIBRARIES_DEBUG ${_FRAMEWORK})
endforeach()

foreach(_SYSTEM_LIB ${boost_SYSTEM_LIBS_DEBUG})
    list(APPEND boost_LIBRARIES_TARGETS_DEBUG ${_SYSTEM_LIB})
    list(APPEND boost_LIBRARIES_DEBUG ${_SYSTEM_LIB})
endforeach()

# We need to add our requirements too
set(boost_LIBRARIES_TARGETS_DEBUG "${boost_LIBRARIES_TARGETS_DEBUG};Boost::headers;Boost::headers;Boost::diagnostic_definitions;Boost::disable_autolinking;Boost::dynamic_linking;boost::_libboost;Boost::system;boost::_libboost;boost::_libboost;boost::_libboost;Boost::exception;Boost::thread;boost::_libboost;Boost::context;Boost::exception;Boost::system;boost::_libboost;boost::_libboost;boost::_libboost;Boost::atomic;Boost::system;boost::_libboost;BZip2::BZip2;ZLIB::ZLIB;Boost::random;Boost::regex;boost::_libboost;Iconv::Iconv;Boost::thread;boost::_libboost;Boost::atomic;Boost::container;Boost::date_time;Boost::exception;Boost::filesystem;Boost::random;Boost::regex;Boost::system;Boost::thread;boost::_libboost;Boost::log;boost::_libboost;Boost::test;boost::_libboost;boost::_libboost;Boost::system;boost::_libboost;boost::_libboost;boost::_libboost;boost::_libboost;Boost::stacktrace;boost::_libboost;libbacktrace::libbacktrace;Boost::stacktrace;boost::_libboost;Boost::stacktrace;boost::_libboost;Boost::stacktrace;boost::_libboost;boost::_libboost;Boost::exception;boost::_libboost;Boost::test;boost::_libboost;Boost::atomic;Boost::chrono;Boost::container;Boost::date_time;Boost::exception;Boost::system;boost::_libboost;Boost::chrono;Boost::system;boost::_libboost;Boost::thread;boost::_libboost;Boost::prg_exec_monitor;Boost::test;Boost::test_exec_monitor;boost::_libboost;Boost::filesystem;Boost::serialization;boost::_libboost;Boost::serialization;boost::_libboost")
set(boost_LIBRARIES_DEBUG "${boost_LIBRARIES_DEBUG};Boost::headers;Boost::headers;Boost::diagnostic_definitions;Boost::disable_autolinking;Boost::dynamic_linking;boost::_libboost;Boost::system;boost::_libboost;boost::_libboost;boost::_libboost;Boost::exception;Boost::thread;boost::_libboost;Boost::context;Boost::exception;Boost::system;boost::_libboost;boost::_libboost;boost::_libboost;Boost::atomic;Boost::system;boost::_libboost;BZip2::BZip2;ZLIB::ZLIB;Boost::random;Boost::regex;boost::_libboost;Iconv::Iconv;Boost::thread;boost::_libboost;Boost::atomic;Boost::container;Boost::date_time;Boost::exception;Boost::filesystem;Boost::random;Boost::regex;Boost::system;Boost::thread;boost::_libboost;Boost::log;boost::_libboost;Boost::test;boost::_libboost;boost::_libboost;Boost::system;boost::_libboost;boost::_libboost;boost::_libboost;boost::_libboost;Boost::stacktrace;boost::_libboost;libbacktrace::libbacktrace;Boost::stacktrace;boost::_libboost;Boost::stacktrace;boost::_libboost;Boost::stacktrace;boost::_libboost;boost::_libboost;Boost::exception;boost::_libboost;Boost::test;boost::_libboost;Boost::atomic;Boost::chrono;Boost::container;Boost::date_time;Boost::exception;Boost::system;boost::_libboost;Boost::chrono;Boost::system;boost::_libboost;Boost::thread;boost::_libboost;Boost::prg_exec_monitor;Boost::test;Boost::test_exec_monitor;boost::_libboost;Boost::filesystem;Boost::serialization;boost::_libboost;Boost::serialization;boost::_libboost")

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${boost_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${boost_BUILD_DIRS_DEBUG} ${CMAKE_PREFIX_PATH})
########## COMPONENT Boost::log_setup FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_log_setup_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_log_setup_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_log_setup_FRAMEWORKS_DEBUG}" "${boost_Boost_log_setup_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_log_setup_LIB_TARGETS_DEBUG "")
set(boost_Boost_log_setup_NOT_USED_DEBUG "")
set(boost_Boost_log_setup_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_log_setup_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_log_setup_SYSTEM_LIBS_DEBUG} ${boost_Boost_log_setup_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_log_setup_LIBS_DEBUG}"
                              "${boost_Boost_log_setup_LIB_DIRS_DEBUG}"
                              "${boost_Boost_log_setup_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_log_setup_NOT_USED_DEBUG
                              boost_Boost_log_setup_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_log_setup")

set(boost_Boost_log_setup_LINK_LIBS_DEBUG ${boost_Boost_log_setup_LIB_TARGETS_DEBUG} ${boost_Boost_log_setup_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::log FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_log_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_log_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_log_FRAMEWORKS_DEBUG}" "${boost_Boost_log_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_log_LIB_TARGETS_DEBUG "")
set(boost_Boost_log_NOT_USED_DEBUG "")
set(boost_Boost_log_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_log_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_log_SYSTEM_LIBS_DEBUG} ${boost_Boost_log_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_log_LIBS_DEBUG}"
                              "${boost_Boost_log_LIB_DIRS_DEBUG}"
                              "${boost_Boost_log_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_log_NOT_USED_DEBUG
                              boost_Boost_log_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_log")

set(boost_Boost_log_LINK_LIBS_DEBUG ${boost_Boost_log_LIB_TARGETS_DEBUG} ${boost_Boost_log_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::locale FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_locale_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_locale_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_locale_FRAMEWORKS_DEBUG}" "${boost_Boost_locale_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_locale_LIB_TARGETS_DEBUG "")
set(boost_Boost_locale_NOT_USED_DEBUG "")
set(boost_Boost_locale_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_locale_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_locale_SYSTEM_LIBS_DEBUG} ${boost_Boost_locale_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_locale_LIBS_DEBUG}"
                              "${boost_Boost_locale_LIB_DIRS_DEBUG}"
                              "${boost_Boost_locale_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_locale_NOT_USED_DEBUG
                              boost_Boost_locale_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_locale")

set(boost_Boost_locale_LINK_LIBS_DEBUG ${boost_Boost_locale_LIB_TARGETS_DEBUG} ${boost_Boost_locale_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::iostreams FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_iostreams_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_iostreams_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_iostreams_FRAMEWORKS_DEBUG}" "${boost_Boost_iostreams_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_iostreams_LIB_TARGETS_DEBUG "")
set(boost_Boost_iostreams_NOT_USED_DEBUG "")
set(boost_Boost_iostreams_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_iostreams_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_iostreams_SYSTEM_LIBS_DEBUG} ${boost_Boost_iostreams_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_iostreams_LIBS_DEBUG}"
                              "${boost_Boost_iostreams_LIB_DIRS_DEBUG}"
                              "${boost_Boost_iostreams_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_iostreams_NOT_USED_DEBUG
                              boost_Boost_iostreams_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_iostreams")

set(boost_Boost_iostreams_LINK_LIBS_DEBUG ${boost_Boost_iostreams_LIB_TARGETS_DEBUG} ${boost_Boost_iostreams_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::contract FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_contract_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_contract_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_contract_FRAMEWORKS_DEBUG}" "${boost_Boost_contract_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_contract_LIB_TARGETS_DEBUG "")
set(boost_Boost_contract_NOT_USED_DEBUG "")
set(boost_Boost_contract_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_contract_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_contract_SYSTEM_LIBS_DEBUG} ${boost_Boost_contract_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_contract_LIBS_DEBUG}"
                              "${boost_Boost_contract_LIB_DIRS_DEBUG}"
                              "${boost_Boost_contract_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_contract_NOT_USED_DEBUG
                              boost_Boost_contract_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_contract")

set(boost_Boost_contract_LINK_LIBS_DEBUG ${boost_Boost_contract_LIB_TARGETS_DEBUG} ${boost_Boost_contract_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::wave FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_wave_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_wave_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_wave_FRAMEWORKS_DEBUG}" "${boost_Boost_wave_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_wave_LIB_TARGETS_DEBUG "")
set(boost_Boost_wave_NOT_USED_DEBUG "")
set(boost_Boost_wave_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_wave_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_wave_SYSTEM_LIBS_DEBUG} ${boost_Boost_wave_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_wave_LIBS_DEBUG}"
                              "${boost_Boost_wave_LIB_DIRS_DEBUG}"
                              "${boost_Boost_wave_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_wave_NOT_USED_DEBUG
                              boost_Boost_wave_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_wave")

set(boost_Boost_wave_LINK_LIBS_DEBUG ${boost_Boost_wave_LIB_TARGETS_DEBUG} ${boost_Boost_wave_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::unit_test_framework FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_unit_test_framework_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_unit_test_framework_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_unit_test_framework_FRAMEWORKS_DEBUG}" "${boost_Boost_unit_test_framework_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_unit_test_framework_LIB_TARGETS_DEBUG "")
set(boost_Boost_unit_test_framework_NOT_USED_DEBUG "")
set(boost_Boost_unit_test_framework_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_unit_test_framework_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_unit_test_framework_SYSTEM_LIBS_DEBUG} ${boost_Boost_unit_test_framework_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_unit_test_framework_LIBS_DEBUG}"
                              "${boost_Boost_unit_test_framework_LIB_DIRS_DEBUG}"
                              "${boost_Boost_unit_test_framework_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_unit_test_framework_NOT_USED_DEBUG
                              boost_Boost_unit_test_framework_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_unit_test_framework")

set(boost_Boost_unit_test_framework_LINK_LIBS_DEBUG ${boost_Boost_unit_test_framework_LIB_TARGETS_DEBUG} ${boost_Boost_unit_test_framework_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::type_erasure FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_type_erasure_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_type_erasure_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_type_erasure_FRAMEWORKS_DEBUG}" "${boost_Boost_type_erasure_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_type_erasure_LIB_TARGETS_DEBUG "")
set(boost_Boost_type_erasure_NOT_USED_DEBUG "")
set(boost_Boost_type_erasure_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_type_erasure_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_type_erasure_SYSTEM_LIBS_DEBUG} ${boost_Boost_type_erasure_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_type_erasure_LIBS_DEBUG}"
                              "${boost_Boost_type_erasure_LIB_DIRS_DEBUG}"
                              "${boost_Boost_type_erasure_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_type_erasure_NOT_USED_DEBUG
                              boost_Boost_type_erasure_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_type_erasure")

set(boost_Boost_type_erasure_LINK_LIBS_DEBUG ${boost_Boost_type_erasure_LIB_TARGETS_DEBUG} ${boost_Boost_type_erasure_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::timer FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_timer_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_timer_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_timer_FRAMEWORKS_DEBUG}" "${boost_Boost_timer_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_timer_LIB_TARGETS_DEBUG "")
set(boost_Boost_timer_NOT_USED_DEBUG "")
set(boost_Boost_timer_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_timer_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_timer_SYSTEM_LIBS_DEBUG} ${boost_Boost_timer_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_timer_LIBS_DEBUG}"
                              "${boost_Boost_timer_LIB_DIRS_DEBUG}"
                              "${boost_Boost_timer_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_timer_NOT_USED_DEBUG
                              boost_Boost_timer_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_timer")

set(boost_Boost_timer_LINK_LIBS_DEBUG ${boost_Boost_timer_LIB_TARGETS_DEBUG} ${boost_Boost_timer_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::thread FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_thread_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_thread_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_thread_FRAMEWORKS_DEBUG}" "${boost_Boost_thread_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_thread_LIB_TARGETS_DEBUG "")
set(boost_Boost_thread_NOT_USED_DEBUG "")
set(boost_Boost_thread_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_thread_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_thread_SYSTEM_LIBS_DEBUG} ${boost_Boost_thread_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_thread_LIBS_DEBUG}"
                              "${boost_Boost_thread_LIB_DIRS_DEBUG}"
                              "${boost_Boost_thread_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_thread_NOT_USED_DEBUG
                              boost_Boost_thread_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_thread")

set(boost_Boost_thread_LINK_LIBS_DEBUG ${boost_Boost_thread_LIB_TARGETS_DEBUG} ${boost_Boost_thread_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::random FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_random_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_random_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_random_FRAMEWORKS_DEBUG}" "${boost_Boost_random_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_random_LIB_TARGETS_DEBUG "")
set(boost_Boost_random_NOT_USED_DEBUG "")
set(boost_Boost_random_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_random_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_random_SYSTEM_LIBS_DEBUG} ${boost_Boost_random_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_random_LIBS_DEBUG}"
                              "${boost_Boost_random_LIB_DIRS_DEBUG}"
                              "${boost_Boost_random_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_random_NOT_USED_DEBUG
                              boost_Boost_random_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_random")

set(boost_Boost_random_LINK_LIBS_DEBUG ${boost_Boost_random_LIB_TARGETS_DEBUG} ${boost_Boost_random_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::prg_exec_monitor FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_prg_exec_monitor_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_prg_exec_monitor_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_prg_exec_monitor_FRAMEWORKS_DEBUG}" "${boost_Boost_prg_exec_monitor_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_prg_exec_monitor_LIB_TARGETS_DEBUG "")
set(boost_Boost_prg_exec_monitor_NOT_USED_DEBUG "")
set(boost_Boost_prg_exec_monitor_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_prg_exec_monitor_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_prg_exec_monitor_SYSTEM_LIBS_DEBUG} ${boost_Boost_prg_exec_monitor_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_prg_exec_monitor_LIBS_DEBUG}"
                              "${boost_Boost_prg_exec_monitor_LIB_DIRS_DEBUG}"
                              "${boost_Boost_prg_exec_monitor_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_prg_exec_monitor_NOT_USED_DEBUG
                              boost_Boost_prg_exec_monitor_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_prg_exec_monitor")

set(boost_Boost_prg_exec_monitor_LINK_LIBS_DEBUG ${boost_Boost_prg_exec_monitor_LIB_TARGETS_DEBUG} ${boost_Boost_prg_exec_monitor_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::filesystem FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_filesystem_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_filesystem_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_filesystem_FRAMEWORKS_DEBUG}" "${boost_Boost_filesystem_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_filesystem_LIB_TARGETS_DEBUG "")
set(boost_Boost_filesystem_NOT_USED_DEBUG "")
set(boost_Boost_filesystem_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_filesystem_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_filesystem_SYSTEM_LIBS_DEBUG} ${boost_Boost_filesystem_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_filesystem_LIBS_DEBUG}"
                              "${boost_Boost_filesystem_LIB_DIRS_DEBUG}"
                              "${boost_Boost_filesystem_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_filesystem_NOT_USED_DEBUG
                              boost_Boost_filesystem_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_filesystem")

set(boost_Boost_filesystem_LINK_LIBS_DEBUG ${boost_Boost_filesystem_LIB_TARGETS_DEBUG} ${boost_Boost_filesystem_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::coroutine FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_coroutine_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_coroutine_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_coroutine_FRAMEWORKS_DEBUG}" "${boost_Boost_coroutine_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_coroutine_LIB_TARGETS_DEBUG "")
set(boost_Boost_coroutine_NOT_USED_DEBUG "")
set(boost_Boost_coroutine_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_coroutine_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_coroutine_SYSTEM_LIBS_DEBUG} ${boost_Boost_coroutine_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_coroutine_LIBS_DEBUG}"
                              "${boost_Boost_coroutine_LIB_DIRS_DEBUG}"
                              "${boost_Boost_coroutine_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_coroutine_NOT_USED_DEBUG
                              boost_Boost_coroutine_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_coroutine")

set(boost_Boost_coroutine_LINK_LIBS_DEBUG ${boost_Boost_coroutine_LIB_TARGETS_DEBUG} ${boost_Boost_coroutine_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::chrono FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_chrono_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_chrono_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_chrono_FRAMEWORKS_DEBUG}" "${boost_Boost_chrono_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_chrono_LIB_TARGETS_DEBUG "")
set(boost_Boost_chrono_NOT_USED_DEBUG "")
set(boost_Boost_chrono_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_chrono_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_chrono_SYSTEM_LIBS_DEBUG} ${boost_Boost_chrono_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_chrono_LIBS_DEBUG}"
                              "${boost_Boost_chrono_LIB_DIRS_DEBUG}"
                              "${boost_Boost_chrono_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_chrono_NOT_USED_DEBUG
                              boost_Boost_chrono_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_chrono")

set(boost_Boost_chrono_LINK_LIBS_DEBUG ${boost_Boost_chrono_LIB_TARGETS_DEBUG} ${boost_Boost_chrono_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::wserialization FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_wserialization_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_wserialization_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_wserialization_FRAMEWORKS_DEBUG}" "${boost_Boost_wserialization_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_wserialization_LIB_TARGETS_DEBUG "")
set(boost_Boost_wserialization_NOT_USED_DEBUG "")
set(boost_Boost_wserialization_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_wserialization_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_wserialization_SYSTEM_LIBS_DEBUG} ${boost_Boost_wserialization_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_wserialization_LIBS_DEBUG}"
                              "${boost_Boost_wserialization_LIB_DIRS_DEBUG}"
                              "${boost_Boost_wserialization_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_wserialization_NOT_USED_DEBUG
                              boost_Boost_wserialization_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_wserialization")

set(boost_Boost_wserialization_LINK_LIBS_DEBUG ${boost_Boost_wserialization_LIB_TARGETS_DEBUG} ${boost_Boost_wserialization_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::test_exec_monitor FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_test_exec_monitor_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_test_exec_monitor_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_test_exec_monitor_FRAMEWORKS_DEBUG}" "${boost_Boost_test_exec_monitor_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_test_exec_monitor_LIB_TARGETS_DEBUG "")
set(boost_Boost_test_exec_monitor_NOT_USED_DEBUG "")
set(boost_Boost_test_exec_monitor_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_test_exec_monitor_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_test_exec_monitor_SYSTEM_LIBS_DEBUG} ${boost_Boost_test_exec_monitor_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_test_exec_monitor_LIBS_DEBUG}"
                              "${boost_Boost_test_exec_monitor_LIB_DIRS_DEBUG}"
                              "${boost_Boost_test_exec_monitor_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_test_exec_monitor_NOT_USED_DEBUG
                              boost_Boost_test_exec_monitor_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_test_exec_monitor")

set(boost_Boost_test_exec_monitor_LINK_LIBS_DEBUG ${boost_Boost_test_exec_monitor_LIB_TARGETS_DEBUG} ${boost_Boost_test_exec_monitor_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::test FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_test_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_test_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_test_FRAMEWORKS_DEBUG}" "${boost_Boost_test_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_test_LIB_TARGETS_DEBUG "")
set(boost_Boost_test_NOT_USED_DEBUG "")
set(boost_Boost_test_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_test_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_test_SYSTEM_LIBS_DEBUG} ${boost_Boost_test_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_test_LIBS_DEBUG}"
                              "${boost_Boost_test_LIB_DIRS_DEBUG}"
                              "${boost_Boost_test_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_test_NOT_USED_DEBUG
                              boost_Boost_test_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_test")

set(boost_Boost_test_LINK_LIBS_DEBUG ${boost_Boost_test_LIB_TARGETS_DEBUG} ${boost_Boost_test_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::system FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_system_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_system_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_system_FRAMEWORKS_DEBUG}" "${boost_Boost_system_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_system_LIB_TARGETS_DEBUG "")
set(boost_Boost_system_NOT_USED_DEBUG "")
set(boost_Boost_system_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_system_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_system_SYSTEM_LIBS_DEBUG} ${boost_Boost_system_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_system_LIBS_DEBUG}"
                              "${boost_Boost_system_LIB_DIRS_DEBUG}"
                              "${boost_Boost_system_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_system_NOT_USED_DEBUG
                              boost_Boost_system_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_system")

set(boost_Boost_system_LINK_LIBS_DEBUG ${boost_Boost_system_LIB_TARGETS_DEBUG} ${boost_Boost_system_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::stacktrace_noop FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_stacktrace_noop_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_stacktrace_noop_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_stacktrace_noop_FRAMEWORKS_DEBUG}" "${boost_Boost_stacktrace_noop_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_stacktrace_noop_LIB_TARGETS_DEBUG "")
set(boost_Boost_stacktrace_noop_NOT_USED_DEBUG "")
set(boost_Boost_stacktrace_noop_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_stacktrace_noop_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_stacktrace_noop_SYSTEM_LIBS_DEBUG} ${boost_Boost_stacktrace_noop_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_stacktrace_noop_LIBS_DEBUG}"
                              "${boost_Boost_stacktrace_noop_LIB_DIRS_DEBUG}"
                              "${boost_Boost_stacktrace_noop_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_stacktrace_noop_NOT_USED_DEBUG
                              boost_Boost_stacktrace_noop_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_stacktrace_noop")

set(boost_Boost_stacktrace_noop_LINK_LIBS_DEBUG ${boost_Boost_stacktrace_noop_LIB_TARGETS_DEBUG} ${boost_Boost_stacktrace_noop_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::stacktrace_basic FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_stacktrace_basic_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_stacktrace_basic_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_stacktrace_basic_FRAMEWORKS_DEBUG}" "${boost_Boost_stacktrace_basic_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_stacktrace_basic_LIB_TARGETS_DEBUG "")
set(boost_Boost_stacktrace_basic_NOT_USED_DEBUG "")
set(boost_Boost_stacktrace_basic_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_stacktrace_basic_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_stacktrace_basic_SYSTEM_LIBS_DEBUG} ${boost_Boost_stacktrace_basic_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_stacktrace_basic_LIBS_DEBUG}"
                              "${boost_Boost_stacktrace_basic_LIB_DIRS_DEBUG}"
                              "${boost_Boost_stacktrace_basic_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_stacktrace_basic_NOT_USED_DEBUG
                              boost_Boost_stacktrace_basic_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_stacktrace_basic")

set(boost_Boost_stacktrace_basic_LINK_LIBS_DEBUG ${boost_Boost_stacktrace_basic_LIB_TARGETS_DEBUG} ${boost_Boost_stacktrace_basic_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::stacktrace_backtrace FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_stacktrace_backtrace_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_stacktrace_backtrace_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_stacktrace_backtrace_FRAMEWORKS_DEBUG}" "${boost_Boost_stacktrace_backtrace_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_stacktrace_backtrace_LIB_TARGETS_DEBUG "")
set(boost_Boost_stacktrace_backtrace_NOT_USED_DEBUG "")
set(boost_Boost_stacktrace_backtrace_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_stacktrace_backtrace_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_stacktrace_backtrace_SYSTEM_LIBS_DEBUG} ${boost_Boost_stacktrace_backtrace_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_stacktrace_backtrace_LIBS_DEBUG}"
                              "${boost_Boost_stacktrace_backtrace_LIB_DIRS_DEBUG}"
                              "${boost_Boost_stacktrace_backtrace_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_stacktrace_backtrace_NOT_USED_DEBUG
                              boost_Boost_stacktrace_backtrace_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_stacktrace_backtrace")

set(boost_Boost_stacktrace_backtrace_LINK_LIBS_DEBUG ${boost_Boost_stacktrace_backtrace_LIB_TARGETS_DEBUG} ${boost_Boost_stacktrace_backtrace_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::stacktrace_addr2line FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_stacktrace_addr2line_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_stacktrace_addr2line_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_stacktrace_addr2line_FRAMEWORKS_DEBUG}" "${boost_Boost_stacktrace_addr2line_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_stacktrace_addr2line_LIB_TARGETS_DEBUG "")
set(boost_Boost_stacktrace_addr2line_NOT_USED_DEBUG "")
set(boost_Boost_stacktrace_addr2line_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_stacktrace_addr2line_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_stacktrace_addr2line_SYSTEM_LIBS_DEBUG} ${boost_Boost_stacktrace_addr2line_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_stacktrace_addr2line_LIBS_DEBUG}"
                              "${boost_Boost_stacktrace_addr2line_LIB_DIRS_DEBUG}"
                              "${boost_Boost_stacktrace_addr2line_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_stacktrace_addr2line_NOT_USED_DEBUG
                              boost_Boost_stacktrace_addr2line_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_stacktrace_addr2line")

set(boost_Boost_stacktrace_addr2line_LINK_LIBS_DEBUG ${boost_Boost_stacktrace_addr2line_LIB_TARGETS_DEBUG} ${boost_Boost_stacktrace_addr2line_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::stacktrace FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_stacktrace_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_stacktrace_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_stacktrace_FRAMEWORKS_DEBUG}" "${boost_Boost_stacktrace_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_stacktrace_LIB_TARGETS_DEBUG "")
set(boost_Boost_stacktrace_NOT_USED_DEBUG "")
set(boost_Boost_stacktrace_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_stacktrace_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_stacktrace_SYSTEM_LIBS_DEBUG} ${boost_Boost_stacktrace_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_stacktrace_LIBS_DEBUG}"
                              "${boost_Boost_stacktrace_LIB_DIRS_DEBUG}"
                              "${boost_Boost_stacktrace_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_stacktrace_NOT_USED_DEBUG
                              boost_Boost_stacktrace_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_stacktrace")

set(boost_Boost_stacktrace_LINK_LIBS_DEBUG ${boost_Boost_stacktrace_LIB_TARGETS_DEBUG} ${boost_Boost_stacktrace_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::serialization FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_serialization_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_serialization_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_serialization_FRAMEWORKS_DEBUG}" "${boost_Boost_serialization_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_serialization_LIB_TARGETS_DEBUG "")
set(boost_Boost_serialization_NOT_USED_DEBUG "")
set(boost_Boost_serialization_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_serialization_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_serialization_SYSTEM_LIBS_DEBUG} ${boost_Boost_serialization_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_serialization_LIBS_DEBUG}"
                              "${boost_Boost_serialization_LIB_DIRS_DEBUG}"
                              "${boost_Boost_serialization_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_serialization_NOT_USED_DEBUG
                              boost_Boost_serialization_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_serialization")

set(boost_Boost_serialization_LINK_LIBS_DEBUG ${boost_Boost_serialization_LIB_TARGETS_DEBUG} ${boost_Boost_serialization_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::regex FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_regex_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_regex_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_regex_FRAMEWORKS_DEBUG}" "${boost_Boost_regex_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_regex_LIB_TARGETS_DEBUG "")
set(boost_Boost_regex_NOT_USED_DEBUG "")
set(boost_Boost_regex_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_regex_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_regex_SYSTEM_LIBS_DEBUG} ${boost_Boost_regex_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_regex_LIBS_DEBUG}"
                              "${boost_Boost_regex_LIB_DIRS_DEBUG}"
                              "${boost_Boost_regex_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_regex_NOT_USED_DEBUG
                              boost_Boost_regex_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_regex")

set(boost_Boost_regex_LINK_LIBS_DEBUG ${boost_Boost_regex_LIB_TARGETS_DEBUG} ${boost_Boost_regex_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::program_options FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_program_options_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_program_options_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_program_options_FRAMEWORKS_DEBUG}" "${boost_Boost_program_options_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_program_options_LIB_TARGETS_DEBUG "")
set(boost_Boost_program_options_NOT_USED_DEBUG "")
set(boost_Boost_program_options_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_program_options_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_program_options_SYSTEM_LIBS_DEBUG} ${boost_Boost_program_options_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_program_options_LIBS_DEBUG}"
                              "${boost_Boost_program_options_LIB_DIRS_DEBUG}"
                              "${boost_Boost_program_options_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_program_options_NOT_USED_DEBUG
                              boost_Boost_program_options_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_program_options")

set(boost_Boost_program_options_LINK_LIBS_DEBUG ${boost_Boost_program_options_LIB_TARGETS_DEBUG} ${boost_Boost_program_options_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::exception FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_exception_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_exception_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_exception_FRAMEWORKS_DEBUG}" "${boost_Boost_exception_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_exception_LIB_TARGETS_DEBUG "")
set(boost_Boost_exception_NOT_USED_DEBUG "")
set(boost_Boost_exception_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_exception_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_exception_SYSTEM_LIBS_DEBUG} ${boost_Boost_exception_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_exception_LIBS_DEBUG}"
                              "${boost_Boost_exception_LIB_DIRS_DEBUG}"
                              "${boost_Boost_exception_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_exception_NOT_USED_DEBUG
                              boost_Boost_exception_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_exception")

set(boost_Boost_exception_LINK_LIBS_DEBUG ${boost_Boost_exception_LIB_TARGETS_DEBUG} ${boost_Boost_exception_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::date_time FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_date_time_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_date_time_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_date_time_FRAMEWORKS_DEBUG}" "${boost_Boost_date_time_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_date_time_LIB_TARGETS_DEBUG "")
set(boost_Boost_date_time_NOT_USED_DEBUG "")
set(boost_Boost_date_time_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_date_time_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_date_time_SYSTEM_LIBS_DEBUG} ${boost_Boost_date_time_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_date_time_LIBS_DEBUG}"
                              "${boost_Boost_date_time_LIB_DIRS_DEBUG}"
                              "${boost_Boost_date_time_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_date_time_NOT_USED_DEBUG
                              boost_Boost_date_time_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_date_time")

set(boost_Boost_date_time_LINK_LIBS_DEBUG ${boost_Boost_date_time_LIB_TARGETS_DEBUG} ${boost_Boost_date_time_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::context FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_context_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_context_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_context_FRAMEWORKS_DEBUG}" "${boost_Boost_context_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_context_LIB_TARGETS_DEBUG "")
set(boost_Boost_context_NOT_USED_DEBUG "")
set(boost_Boost_context_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_context_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_context_SYSTEM_LIBS_DEBUG} ${boost_Boost_context_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_context_LIBS_DEBUG}"
                              "${boost_Boost_context_LIB_DIRS_DEBUG}"
                              "${boost_Boost_context_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_context_NOT_USED_DEBUG
                              boost_Boost_context_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_context")

set(boost_Boost_context_LINK_LIBS_DEBUG ${boost_Boost_context_LIB_TARGETS_DEBUG} ${boost_Boost_context_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::container FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_container_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_container_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_container_FRAMEWORKS_DEBUG}" "${boost_Boost_container_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_container_LIB_TARGETS_DEBUG "")
set(boost_Boost_container_NOT_USED_DEBUG "")
set(boost_Boost_container_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_container_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_container_SYSTEM_LIBS_DEBUG} ${boost_Boost_container_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_container_LIBS_DEBUG}"
                              "${boost_Boost_container_LIB_DIRS_DEBUG}"
                              "${boost_Boost_container_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_container_NOT_USED_DEBUG
                              boost_Boost_container_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_container")

set(boost_Boost_container_LINK_LIBS_DEBUG ${boost_Boost_container_LIB_TARGETS_DEBUG} ${boost_Boost_container_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::atomic FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_atomic_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_atomic_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_atomic_FRAMEWORKS_DEBUG}" "${boost_Boost_atomic_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_atomic_LIB_TARGETS_DEBUG "")
set(boost_Boost_atomic_NOT_USED_DEBUG "")
set(boost_Boost_atomic_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_atomic_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_atomic_SYSTEM_LIBS_DEBUG} ${boost_Boost_atomic_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_atomic_LIBS_DEBUG}"
                              "${boost_Boost_atomic_LIB_DIRS_DEBUG}"
                              "${boost_Boost_atomic_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_atomic_NOT_USED_DEBUG
                              boost_Boost_atomic_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_atomic")

set(boost_Boost_atomic_LINK_LIBS_DEBUG ${boost_Boost_atomic_LIB_TARGETS_DEBUG} ${boost_Boost_atomic_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT boost::_libboost FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_boost__libboost_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_boost__libboost_FRAMEWORKS_FOUND_DEBUG "${boost_boost__libboost_FRAMEWORKS_DEBUG}" "${boost_boost__libboost_FRAMEWORK_DIRS_DEBUG}")

set(boost_boost__libboost_LIB_TARGETS_DEBUG "")
set(boost_boost__libboost_NOT_USED_DEBUG "")
set(boost_boost__libboost_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_boost__libboost_FRAMEWORKS_FOUND_DEBUG} ${boost_boost__libboost_SYSTEM_LIBS_DEBUG} ${boost_boost__libboost_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_boost__libboost_LIBS_DEBUG}"
                              "${boost_boost__libboost_LIB_DIRS_DEBUG}"
                              "${boost_boost__libboost_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_boost__libboost_NOT_USED_DEBUG
                              boost_boost__libboost_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_boost__libboost")

set(boost_boost__libboost_LINK_LIBS_DEBUG ${boost_boost__libboost_LIB_TARGETS_DEBUG} ${boost_boost__libboost_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::dynamic_linking FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_dynamic_linking_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_dynamic_linking_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_dynamic_linking_FRAMEWORKS_DEBUG}" "${boost_Boost_dynamic_linking_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_dynamic_linking_LIB_TARGETS_DEBUG "")
set(boost_Boost_dynamic_linking_NOT_USED_DEBUG "")
set(boost_Boost_dynamic_linking_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_dynamic_linking_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_dynamic_linking_SYSTEM_LIBS_DEBUG} ${boost_Boost_dynamic_linking_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_dynamic_linking_LIBS_DEBUG}"
                              "${boost_Boost_dynamic_linking_LIB_DIRS_DEBUG}"
                              "${boost_Boost_dynamic_linking_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_dynamic_linking_NOT_USED_DEBUG
                              boost_Boost_dynamic_linking_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_dynamic_linking")

set(boost_Boost_dynamic_linking_LINK_LIBS_DEBUG ${boost_Boost_dynamic_linking_LIB_TARGETS_DEBUG} ${boost_Boost_dynamic_linking_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::disable_autolinking FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_disable_autolinking_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_disable_autolinking_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_disable_autolinking_FRAMEWORKS_DEBUG}" "${boost_Boost_disable_autolinking_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_disable_autolinking_LIB_TARGETS_DEBUG "")
set(boost_Boost_disable_autolinking_NOT_USED_DEBUG "")
set(boost_Boost_disable_autolinking_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_disable_autolinking_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_disable_autolinking_SYSTEM_LIBS_DEBUG} ${boost_Boost_disable_autolinking_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_disable_autolinking_LIBS_DEBUG}"
                              "${boost_Boost_disable_autolinking_LIB_DIRS_DEBUG}"
                              "${boost_Boost_disable_autolinking_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_disable_autolinking_NOT_USED_DEBUG
                              boost_Boost_disable_autolinking_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_disable_autolinking")

set(boost_Boost_disable_autolinking_LINK_LIBS_DEBUG ${boost_Boost_disable_autolinking_LIB_TARGETS_DEBUG} ${boost_Boost_disable_autolinking_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::diagnostic_definitions FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_diagnostic_definitions_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_diagnostic_definitions_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_diagnostic_definitions_FRAMEWORKS_DEBUG}" "${boost_Boost_diagnostic_definitions_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_diagnostic_definitions_LIB_TARGETS_DEBUG "")
set(boost_Boost_diagnostic_definitions_NOT_USED_DEBUG "")
set(boost_Boost_diagnostic_definitions_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_diagnostic_definitions_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_diagnostic_definitions_SYSTEM_LIBS_DEBUG} ${boost_Boost_diagnostic_definitions_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_diagnostic_definitions_LIBS_DEBUG}"
                              "${boost_Boost_diagnostic_definitions_LIB_DIRS_DEBUG}"
                              "${boost_Boost_diagnostic_definitions_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_diagnostic_definitions_NOT_USED_DEBUG
                              boost_Boost_diagnostic_definitions_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_diagnostic_definitions")

set(boost_Boost_diagnostic_definitions_LINK_LIBS_DEBUG ${boost_Boost_diagnostic_definitions_LIB_TARGETS_DEBUG} ${boost_Boost_diagnostic_definitions_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::boost FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_boost_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_boost_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_boost_FRAMEWORKS_DEBUG}" "${boost_Boost_boost_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_boost_LIB_TARGETS_DEBUG "")
set(boost_Boost_boost_NOT_USED_DEBUG "")
set(boost_Boost_boost_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_boost_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_boost_SYSTEM_LIBS_DEBUG} ${boost_Boost_boost_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_boost_LIBS_DEBUG}"
                              "${boost_Boost_boost_LIB_DIRS_DEBUG}"
                              "${boost_Boost_boost_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_boost_NOT_USED_DEBUG
                              boost_Boost_boost_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_boost")

set(boost_Boost_boost_LINK_LIBS_DEBUG ${boost_Boost_boost_LIB_TARGETS_DEBUG} ${boost_Boost_boost_LIBS_FRAMEWORKS_DEPS_DEBUG})
########## COMPONENT Boost::headers FIND LIBRARIES & FRAMEWORKS / DYNAMIC VARS #############

set(boost_Boost_headers_FRAMEWORKS_FOUND_DEBUG "")
conan_find_apple_frameworks(boost_Boost_headers_FRAMEWORKS_FOUND_DEBUG "${boost_Boost_headers_FRAMEWORKS_DEBUG}" "${boost_Boost_headers_FRAMEWORK_DIRS_DEBUG}")

set(boost_Boost_headers_LIB_TARGETS_DEBUG "")
set(boost_Boost_headers_NOT_USED_DEBUG "")
set(boost_Boost_headers_LIBS_FRAMEWORKS_DEPS_DEBUG ${boost_Boost_headers_FRAMEWORKS_FOUND_DEBUG} ${boost_Boost_headers_SYSTEM_LIBS_DEBUG} ${boost_Boost_headers_DEPENDENCIES_DEBUG})
conan_package_library_targets("${boost_Boost_headers_LIBS_DEBUG}"
                              "${boost_Boost_headers_LIB_DIRS_DEBUG}"
                              "${boost_Boost_headers_LIBS_FRAMEWORKS_DEPS_DEBUG}"
                              boost_Boost_headers_NOT_USED_DEBUG
                              boost_Boost_headers_LIB_TARGETS_DEBUG
                              "_DEBUG"
                              "boost_Boost_headers")

set(boost_Boost_headers_LINK_LIBS_DEBUG ${boost_Boost_headers_LIB_TARGETS_DEBUG} ${boost_Boost_headers_LIBS_FRAMEWORKS_DEPS_DEBUG})


########## GLOBAL TARGET PROPERTIES Debug ########################################
set_property(TARGET boost::boost
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_LIBRARIES_TARGETS_DEBUG}
                                           ${boost_OBJECTS_DEBUG}> APPEND)
set_property(TARGET boost::boost
             PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET boost::boost
             PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET boost::boost
             PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET boost::boost
             PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:${boost_COMPILE_OPTIONS_DEBUG}> APPEND)

########## COMPONENTS TARGET PROPERTIES Debug ########################################
########## COMPONENT Boost::log_setup TARGET PROPERTIES ######################################
set_property(TARGET Boost::log_setup PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_log_setup_LINK_LIBS_DEBUG}
             ${boost_Boost_log_setup_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::log_setup PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_log_setup_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::log_setup PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_log_setup_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::log_setup PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_log_setup_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::log_setup PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_log_setup_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_log_setup_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_log_setup_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::log TARGET PROPERTIES ######################################
set_property(TARGET Boost::log PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_log_LINK_LIBS_DEBUG}
             ${boost_Boost_log_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::log PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_log_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::log PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_log_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::log PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_log_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::log PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_log_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_log_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_log_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::locale TARGET PROPERTIES ######################################
set_property(TARGET Boost::locale PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_locale_LINK_LIBS_DEBUG}
             ${boost_Boost_locale_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::locale PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_locale_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::locale PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_locale_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::locale PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_locale_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::locale PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_locale_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_locale_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_locale_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::iostreams TARGET PROPERTIES ######################################
set_property(TARGET Boost::iostreams PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_iostreams_LINK_LIBS_DEBUG}
             ${boost_Boost_iostreams_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::iostreams PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_iostreams_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::iostreams PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_iostreams_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::iostreams PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_iostreams_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::iostreams PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_iostreams_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_iostreams_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_iostreams_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::contract TARGET PROPERTIES ######################################
set_property(TARGET Boost::contract PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_contract_LINK_LIBS_DEBUG}
             ${boost_Boost_contract_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::contract PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_contract_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::contract PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_contract_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::contract PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_contract_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::contract PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_contract_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_contract_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_contract_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::wave TARGET PROPERTIES ######################################
set_property(TARGET Boost::wave PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_wave_LINK_LIBS_DEBUG}
             ${boost_Boost_wave_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::wave PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_wave_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::wave PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_wave_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::wave PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_wave_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::wave PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_wave_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_wave_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_wave_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::unit_test_framework TARGET PROPERTIES ######################################
set_property(TARGET Boost::unit_test_framework PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_unit_test_framework_LINK_LIBS_DEBUG}
             ${boost_Boost_unit_test_framework_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::unit_test_framework PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_unit_test_framework_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::unit_test_framework PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_unit_test_framework_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::unit_test_framework PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_unit_test_framework_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::unit_test_framework PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_unit_test_framework_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_unit_test_framework_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_unit_test_framework_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::type_erasure TARGET PROPERTIES ######################################
set_property(TARGET Boost::type_erasure PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_type_erasure_LINK_LIBS_DEBUG}
             ${boost_Boost_type_erasure_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::type_erasure PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_type_erasure_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::type_erasure PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_type_erasure_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::type_erasure PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_type_erasure_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::type_erasure PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_type_erasure_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_type_erasure_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_type_erasure_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::timer TARGET PROPERTIES ######################################
set_property(TARGET Boost::timer PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_timer_LINK_LIBS_DEBUG}
             ${boost_Boost_timer_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::timer PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_timer_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::timer PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_timer_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::timer PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_timer_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::timer PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_timer_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_timer_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_timer_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::thread TARGET PROPERTIES ######################################
set_property(TARGET Boost::thread PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_thread_LINK_LIBS_DEBUG}
             ${boost_Boost_thread_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::thread PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_thread_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::thread PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_thread_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::thread PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_thread_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::thread PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_thread_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_thread_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_thread_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::random TARGET PROPERTIES ######################################
set_property(TARGET Boost::random PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_random_LINK_LIBS_DEBUG}
             ${boost_Boost_random_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::random PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_random_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::random PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_random_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::random PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_random_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::random PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_random_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_random_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_random_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::prg_exec_monitor TARGET PROPERTIES ######################################
set_property(TARGET Boost::prg_exec_monitor PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_prg_exec_monitor_LINK_LIBS_DEBUG}
             ${boost_Boost_prg_exec_monitor_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::prg_exec_monitor PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_prg_exec_monitor_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::prg_exec_monitor PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_prg_exec_monitor_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::prg_exec_monitor PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_prg_exec_monitor_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::prg_exec_monitor PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_prg_exec_monitor_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_prg_exec_monitor_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_prg_exec_monitor_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::filesystem TARGET PROPERTIES ######################################
set_property(TARGET Boost::filesystem PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_filesystem_LINK_LIBS_DEBUG}
             ${boost_Boost_filesystem_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::filesystem PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_filesystem_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::filesystem PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_filesystem_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::filesystem PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_filesystem_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::filesystem PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_filesystem_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_filesystem_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_filesystem_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::coroutine TARGET PROPERTIES ######################################
set_property(TARGET Boost::coroutine PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_coroutine_LINK_LIBS_DEBUG}
             ${boost_Boost_coroutine_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::coroutine PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_coroutine_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::coroutine PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_coroutine_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::coroutine PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_coroutine_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::coroutine PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_coroutine_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_coroutine_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_coroutine_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::chrono TARGET PROPERTIES ######################################
set_property(TARGET Boost::chrono PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_chrono_LINK_LIBS_DEBUG}
             ${boost_Boost_chrono_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::chrono PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_chrono_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::chrono PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_chrono_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::chrono PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_chrono_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::chrono PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_chrono_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_chrono_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_chrono_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::wserialization TARGET PROPERTIES ######################################
set_property(TARGET Boost::wserialization PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_wserialization_LINK_LIBS_DEBUG}
             ${boost_Boost_wserialization_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::wserialization PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_wserialization_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::wserialization PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_wserialization_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::wserialization PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_wserialization_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::wserialization PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_wserialization_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_wserialization_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_wserialization_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::test_exec_monitor TARGET PROPERTIES ######################################
set_property(TARGET Boost::test_exec_monitor PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_test_exec_monitor_LINK_LIBS_DEBUG}
             ${boost_Boost_test_exec_monitor_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::test_exec_monitor PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_test_exec_monitor_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::test_exec_monitor PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_test_exec_monitor_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::test_exec_monitor PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_test_exec_monitor_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::test_exec_monitor PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_test_exec_monitor_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_test_exec_monitor_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_test_exec_monitor_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::test TARGET PROPERTIES ######################################
set_property(TARGET Boost::test PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_test_LINK_LIBS_DEBUG}
             ${boost_Boost_test_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::test PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_test_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::test PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_test_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::test PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_test_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::test PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_test_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_test_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_test_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::system TARGET PROPERTIES ######################################
set_property(TARGET Boost::system PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_system_LINK_LIBS_DEBUG}
             ${boost_Boost_system_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::system PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_system_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::system PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_system_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::system PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_system_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::system PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_system_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_system_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_system_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::stacktrace_noop TARGET PROPERTIES ######################################
set_property(TARGET Boost::stacktrace_noop PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_stacktrace_noop_LINK_LIBS_DEBUG}
             ${boost_Boost_stacktrace_noop_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::stacktrace_noop PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_stacktrace_noop_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::stacktrace_noop PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_stacktrace_noop_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::stacktrace_noop PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_stacktrace_noop_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::stacktrace_noop PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_stacktrace_noop_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_stacktrace_noop_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_stacktrace_noop_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::stacktrace_basic TARGET PROPERTIES ######################################
set_property(TARGET Boost::stacktrace_basic PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_stacktrace_basic_LINK_LIBS_DEBUG}
             ${boost_Boost_stacktrace_basic_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::stacktrace_basic PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_stacktrace_basic_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::stacktrace_basic PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_stacktrace_basic_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::stacktrace_basic PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_stacktrace_basic_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::stacktrace_basic PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_stacktrace_basic_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_stacktrace_basic_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_stacktrace_basic_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::stacktrace_backtrace TARGET PROPERTIES ######################################
set_property(TARGET Boost::stacktrace_backtrace PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_stacktrace_backtrace_LINK_LIBS_DEBUG}
             ${boost_Boost_stacktrace_backtrace_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::stacktrace_backtrace PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_stacktrace_backtrace_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::stacktrace_backtrace PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_stacktrace_backtrace_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::stacktrace_backtrace PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_stacktrace_backtrace_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::stacktrace_backtrace PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_stacktrace_backtrace_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_stacktrace_backtrace_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_stacktrace_backtrace_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::stacktrace_addr2line TARGET PROPERTIES ######################################
set_property(TARGET Boost::stacktrace_addr2line PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_stacktrace_addr2line_LINK_LIBS_DEBUG}
             ${boost_Boost_stacktrace_addr2line_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::stacktrace_addr2line PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_stacktrace_addr2line_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::stacktrace_addr2line PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_stacktrace_addr2line_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::stacktrace_addr2line PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_stacktrace_addr2line_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::stacktrace_addr2line PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_stacktrace_addr2line_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_stacktrace_addr2line_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_stacktrace_addr2line_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::stacktrace TARGET PROPERTIES ######################################
set_property(TARGET Boost::stacktrace PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_stacktrace_LINK_LIBS_DEBUG}
             ${boost_Boost_stacktrace_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::stacktrace PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_stacktrace_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::stacktrace PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_stacktrace_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::stacktrace PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_stacktrace_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::stacktrace PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_stacktrace_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_stacktrace_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_stacktrace_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::serialization TARGET PROPERTIES ######################################
set_property(TARGET Boost::serialization PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_serialization_LINK_LIBS_DEBUG}
             ${boost_Boost_serialization_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::serialization PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_serialization_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::serialization PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_serialization_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::serialization PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_serialization_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::serialization PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_serialization_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_serialization_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_serialization_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::regex TARGET PROPERTIES ######################################
set_property(TARGET Boost::regex PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_regex_LINK_LIBS_DEBUG}
             ${boost_Boost_regex_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::regex PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_regex_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::regex PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_regex_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::regex PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_regex_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::regex PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_regex_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_regex_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_regex_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::program_options TARGET PROPERTIES ######################################
set_property(TARGET Boost::program_options PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_program_options_LINK_LIBS_DEBUG}
             ${boost_Boost_program_options_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::program_options PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_program_options_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::program_options PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_program_options_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::program_options PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_program_options_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::program_options PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_program_options_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_program_options_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_program_options_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::exception TARGET PROPERTIES ######################################
set_property(TARGET Boost::exception PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_exception_LINK_LIBS_DEBUG}
             ${boost_Boost_exception_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::exception PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_exception_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::exception PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_exception_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::exception PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_exception_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::exception PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_exception_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_exception_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_exception_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::date_time TARGET PROPERTIES ######################################
set_property(TARGET Boost::date_time PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_date_time_LINK_LIBS_DEBUG}
             ${boost_Boost_date_time_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::date_time PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_date_time_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::date_time PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_date_time_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::date_time PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_date_time_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::date_time PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_date_time_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_date_time_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_date_time_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::context TARGET PROPERTIES ######################################
set_property(TARGET Boost::context PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_context_LINK_LIBS_DEBUG}
             ${boost_Boost_context_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::context PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_context_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::context PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_context_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::context PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_context_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::context PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_context_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_context_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_context_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::container TARGET PROPERTIES ######################################
set_property(TARGET Boost::container PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_container_LINK_LIBS_DEBUG}
             ${boost_Boost_container_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::container PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_container_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::container PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_container_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::container PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_container_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::container PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_container_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_container_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_container_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::atomic TARGET PROPERTIES ######################################
set_property(TARGET Boost::atomic PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_atomic_LINK_LIBS_DEBUG}
             ${boost_Boost_atomic_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::atomic PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_atomic_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::atomic PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_atomic_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::atomic PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_atomic_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::atomic PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_atomic_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_atomic_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_atomic_TARGET_PROPERTIES TRUE)
########## COMPONENT boost::_libboost TARGET PROPERTIES ######################################
set_property(TARGET boost::_libboost PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_boost__libboost_LINK_LIBS_DEBUG}
             ${boost_boost__libboost_OBJECTS_DEBUG}> APPEND)
set_property(TARGET boost::_libboost PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_boost__libboost_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET boost::_libboost PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_boost__libboost_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET boost::_libboost PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_boost__libboost_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET boost::_libboost PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_boost__libboost_COMPILE_OPTIONS_C_DEBUG}
             ${boost_boost__libboost_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_boost__libboost_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::dynamic_linking TARGET PROPERTIES ######################################
set_property(TARGET Boost::dynamic_linking PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_dynamic_linking_LINK_LIBS_DEBUG}
             ${boost_Boost_dynamic_linking_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::dynamic_linking PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_dynamic_linking_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::dynamic_linking PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_dynamic_linking_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::dynamic_linking PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_dynamic_linking_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::dynamic_linking PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_dynamic_linking_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_dynamic_linking_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_dynamic_linking_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::disable_autolinking TARGET PROPERTIES ######################################
set_property(TARGET Boost::disable_autolinking PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_disable_autolinking_LINK_LIBS_DEBUG}
             ${boost_Boost_disable_autolinking_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::disable_autolinking PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_disable_autolinking_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::disable_autolinking PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_disable_autolinking_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::disable_autolinking PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_disable_autolinking_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::disable_autolinking PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_disable_autolinking_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_disable_autolinking_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_disable_autolinking_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::diagnostic_definitions TARGET PROPERTIES ######################################
set_property(TARGET Boost::diagnostic_definitions PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_diagnostic_definitions_LINK_LIBS_DEBUG}
             ${boost_Boost_diagnostic_definitions_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::diagnostic_definitions PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_diagnostic_definitions_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::diagnostic_definitions PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_diagnostic_definitions_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::diagnostic_definitions PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_diagnostic_definitions_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::diagnostic_definitions PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_diagnostic_definitions_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_diagnostic_definitions_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_diagnostic_definitions_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::boost TARGET PROPERTIES ######################################
set_property(TARGET Boost::boost PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_boost_LINK_LIBS_DEBUG}
             ${boost_Boost_boost_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::boost PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_boost_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::boost PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_boost_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::boost PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_boost_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::boost PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_boost_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_boost_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_boost_TARGET_PROPERTIES TRUE)
########## COMPONENT Boost::headers TARGET PROPERTIES ######################################
set_property(TARGET Boost::headers PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${boost_Boost_headers_LINK_LIBS_DEBUG}
             ${boost_Boost_headers_OBJECTS_DEBUG}> APPEND)
set_property(TARGET Boost::headers PROPERTY INTERFACE_LINK_OPTIONS
             $<$<CONFIG:Debug>:${boost_Boost_headers_LINKER_FLAGS_DEBUG}> APPEND)
set_property(TARGET Boost::headers PROPERTY INTERFACE_INCLUDE_DIRECTORIES
             $<$<CONFIG:Debug>:${boost_Boost_headers_INCLUDE_DIRS_DEBUG}> APPEND)
set_property(TARGET Boost::headers PROPERTY INTERFACE_COMPILE_DEFINITIONS
             $<$<CONFIG:Debug>:${boost_Boost_headers_COMPILE_DEFINITIONS_DEBUG}> APPEND)
set_property(TARGET Boost::headers PROPERTY INTERFACE_COMPILE_OPTIONS
             $<$<CONFIG:Debug>:
             ${boost_Boost_headers_COMPILE_OPTIONS_C_DEBUG}
             ${boost_Boost_headers_COMPILE_OPTIONS_CXX_DEBUG}> APPEND)
set(boost_Boost_headers_TARGET_PROPERTIES TRUE)
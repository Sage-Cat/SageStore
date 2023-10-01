# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(sqlite3_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(sqlite3_FRAMEWORKS_FOUND_RELEASE "${sqlite3_FRAMEWORKS_RELEASE}" "${sqlite3_FRAMEWORK_DIRS_RELEASE}")

set(sqlite3_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET sqlite3_DEPS_TARGET)
    add_library(sqlite3_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET sqlite3_DEPS_TARGET
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${sqlite3_FRAMEWORKS_FOUND_RELEASE}>
             $<$<CONFIG:Release>:${sqlite3_SYSTEM_LIBS_RELEASE}>
             $<$<CONFIG:Release>:>
             APPEND)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### sqlite3_DEPS_TARGET to all of them
conan_package_library_targets("${sqlite3_LIBS_RELEASE}"    # libraries
                              "${sqlite3_LIB_DIRS_RELEASE}" # package_libdir
                              "${sqlite3_BIN_DIRS_RELEASE}" # package_bindir
                              "${sqlite3_LIBRARY_TYPE_RELEASE}"
                              "${sqlite3_IS_HOST_WINDOWS_RELEASE}"
                              sqlite3_DEPS_TARGET
                              sqlite3_LIBRARIES_TARGETS  # out_libraries_targets
                              "_RELEASE"
                              "sqlite3"    # package_name
                              "${sqlite3_NO_SONAME_MODE_RELEASE}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${sqlite3_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})

########## COMPONENTS TARGET PROPERTIES Release ########################################

    ########## COMPONENT SQLite::SQLite3 #############

        set(sqlite3_SQLite_SQLite3_FRAMEWORKS_FOUND_RELEASE "")
        conan_find_apple_frameworks(sqlite3_SQLite_SQLite3_FRAMEWORKS_FOUND_RELEASE "${sqlite3_SQLite_SQLite3_FRAMEWORKS_RELEASE}" "${sqlite3_SQLite_SQLite3_FRAMEWORK_DIRS_RELEASE}")

        set(sqlite3_SQLite_SQLite3_LIBRARIES_TARGETS "")

        ######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
        if(NOT TARGET sqlite3_SQLite_SQLite3_DEPS_TARGET)
            add_library(sqlite3_SQLite_SQLite3_DEPS_TARGET INTERFACE IMPORTED)
        endif()

        set_property(TARGET sqlite3_SQLite_SQLite3_DEPS_TARGET
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Release>:${sqlite3_SQLite_SQLite3_FRAMEWORKS_FOUND_RELEASE}>
                     $<$<CONFIG:Release>:${sqlite3_SQLite_SQLite3_SYSTEM_LIBS_RELEASE}>
                     $<$<CONFIG:Release>:${sqlite3_SQLite_SQLite3_DEPENDENCIES_RELEASE}>
                     APPEND)

        ####### Find the libraries declared in cpp_info.component["xxx"].libs,
        ####### create an IMPORTED target for each one and link the 'sqlite3_SQLite_SQLite3_DEPS_TARGET' to all of them
        conan_package_library_targets("${sqlite3_SQLite_SQLite3_LIBS_RELEASE}"
                              "${sqlite3_SQLite_SQLite3_LIB_DIRS_RELEASE}"
                              "${sqlite3_SQLite_SQLite3_BIN_DIRS_RELEASE}" # package_bindir
                              "${sqlite3_SQLite_SQLite3_LIBRARY_TYPE_RELEASE}"
                              "${sqlite3_SQLite_SQLite3_IS_HOST_WINDOWS_RELEASE}"
                              sqlite3_SQLite_SQLite3_DEPS_TARGET
                              sqlite3_SQLite_SQLite3_LIBRARIES_TARGETS
                              "_RELEASE"
                              "sqlite3_SQLite_SQLite3"
                              "${sqlite3_SQLite_SQLite3_NO_SONAME_MODE_RELEASE}")


        ########## TARGET PROPERTIES #####################################
        set_property(TARGET SQLite::SQLite3
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     $<$<CONFIG:Release>:${sqlite3_SQLite_SQLite3_OBJECTS_RELEASE}>
                     $<$<CONFIG:Release>:${sqlite3_SQLite_SQLite3_LIBRARIES_TARGETS}>
                     APPEND)

        if("${sqlite3_SQLite_SQLite3_LIBS_RELEASE}" STREQUAL "")
            # If the component is not declaring any "cpp_info.components['foo'].libs" the system, frameworks etc are not
            # linked to the imported targets and we need to do it to the global target
            set_property(TARGET SQLite::SQLite3
                         PROPERTY INTERFACE_LINK_LIBRARIES
                         sqlite3_SQLite_SQLite3_DEPS_TARGET
                         APPEND)
        endif()

        set_property(TARGET SQLite::SQLite3 PROPERTY INTERFACE_LINK_OPTIONS
                     $<$<CONFIG:Release>:${sqlite3_SQLite_SQLite3_LINKER_FLAGS_RELEASE}> APPEND)
        set_property(TARGET SQLite::SQLite3 PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                     $<$<CONFIG:Release>:${sqlite3_SQLite_SQLite3_INCLUDE_DIRS_RELEASE}> APPEND)
        set_property(TARGET SQLite::SQLite3 PROPERTY INTERFACE_LINK_DIRECTORIES
                     $<$<CONFIG:Release>:${sqlite3_SQLite_SQLite3_LIB_DIRS_RELEASE}> APPEND)
        set_property(TARGET SQLite::SQLite3 PROPERTY INTERFACE_COMPILE_DEFINITIONS
                     $<$<CONFIG:Release>:${sqlite3_SQLite_SQLite3_COMPILE_DEFINITIONS_RELEASE}> APPEND)
        set_property(TARGET SQLite::SQLite3 PROPERTY INTERFACE_COMPILE_OPTIONS
                     $<$<CONFIG:Release>:${sqlite3_SQLite_SQLite3_COMPILE_OPTIONS_RELEASE}> APPEND)

    ########## AGGREGATED GLOBAL TARGET WITH THE COMPONENTS #####################
    set_property(TARGET SQLite::SQLite3 PROPERTY INTERFACE_LINK_LIBRARIES SQLite::SQLite3 APPEND)

########## For the modules (FindXXX)
set(sqlite3_LIBRARIES_RELEASE SQLite::SQLite3)

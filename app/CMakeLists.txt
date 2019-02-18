cmake_minimum_required(VERSION 3.0.0)
project("Jni VFV")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)

set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH 1)
set(PKG_CONFIG_PATH_ARM "" CACHE PATH "Path to pkgconfig files for arm architecture")

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

function(findSO _prefix _pkgPrefix)
    unset(_search_paths)
    foreach (flag IN LISTS ${_pkgPrefix}_LDFLAGS)
        if(flag MATCHES "^-L(.*)")
            list(APPEND _search_paths ${CMAKE_MATCH_1})
            continue()
        endif()
        if (flag MATCHES "^-l(.*)")
            set(_pkg_search "${CMAKE_MATCH_1}")
        else()
            continue()
        endif()

        if(_search_paths)
            MESSAGE(STATUS ${_search_paths})
            # Firstly search in -L paths
            find_file(${_prefix}_file
                    NAMES lib${${_pkgPrefix}_LIBRARIES}.so
                    HINTS ${_search_paths} NO_DEFAULT_PATH)
        endif()
        find_file(${_prefix}_file
                NAMES lib${${_pkgPrefix}_LIBRARIES}.so
                ${_find_opts})
        list(APPEND _libs "${${_prefix}_file}")
    endforeach()

    set(${_prefix} "${_libs}" PARENT_SCOPE)
endfunction()

#Define native-lib (sources + headers)
include_directories(src/main/cpp/include)
FILE(GLOB_RECURSE SOURCES src/main/cpp/src/*.cpp)
FILE(GLOB_RECURSE HEADERS src/main/cpp/include/*.h)

#Search for pkgconfig external libraries
MESSAGE(STATUS "Compiling for ${ANDROID_ABI}")
if(${ANDROID_ABI} STREQUAL "arm" OR ${ANDROID_ABI} STREQUAL "armeabi-v7a" OR ${ANDROID_ABI} STREQUAL "armeabi")
    set(ENV{PKG_CONFIG_PATH} "${CMAKE_PREFIX_PATH_ARM}")
elseif(${ANDROID_ABI} STREQUAL "x86")
    set(ENV{PKG_CONFIG_PATH} "${CMAKE_PREFIX_PATH_X86}")
endif()

#Check module in PKG
find_package(PkgConfig              REQUIRED)
PKG_CHECK_MODULES(LIB3DS            REQUIRED lib3ds)
PKG_CHECK_MODULES(GLM               REQUIRED glm)
PKG_CHECK_MODULES(SERENO_VTK_PARSER REQUIRED serenoVTKParser)

#Find android internal libraries
find_library(log-lib     log)
find_library(android-lib android)
find_library(egl-lib     EGL)
find_library(gles-lib    GLESv3)

#######################
#Add external libraries
#######################

#3ds lib
findSO(3DS_SO LIB3DS)
add_library(3ds-lib SHARED IMPORTED)
set_target_properties(3ds-lib PROPERTIES
        IMPORTED_LOCATION "${3DS_SO}")

#sereno vtk parser
findSO(SERENO_VTK_PARSER_SO SERENO_VTK_PARSER)
MESSAGE(STATUS ${SERENO_VTK_PARSER_SO})
add_library(sereno_vtk_parser-lib SHARED IMPORTED)
set_target_properties(sereno_vtk_parser-lib PROPERTIES
        IMPORTED_LOCATION "${SERENO_VTK_PARSER_SO}")

add_library(native-lib SHARED ${SOURCES} ${HEADERS})
target_compile_options(native-lib PRIVATE ${LIB3DS_CFLAGS} ${GLM_CFLAGS} ${SERENO_VTK_PARSER_CFLAGS})
target_link_libraries(native-lib ${GLM_LDFLAGS} 3ds-lib sereno_vtk_parser-lib ${log-lib} ${egl-lib} ${gles-lib} ${android-lib})

include(AndroidNdkGdb)
android_ndk_gdb_enable()
android_ndk_gdb_debuggable(native-lib)

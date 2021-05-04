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
    MESSAGE(STATUS "${${_pkgPrefix}_LDFLAGS}")
    foreach (flag IN LISTS ${_pkgPrefix}_LDFLAGS)
        if(flag MATCHES "^-L(.*)")
            list(APPEND _search_paths ${CMAKE_MATCH_1})
            continue()
        endif()
        if (flag MATCHES "^-l(.*)")
            set(_pkg_search "${CMAKE_MATCH_1}")

            if(_search_paths)
                # Firstly search in -L paths
                find_file(${_pkg_search}_file
                        NAMES lib${_pkg_search}.so
                        PATHS ${_search_paths}
                        NO_DEFAULT_PATH)
                MESSAGE(STATUS ${_pkg_search}.so --> ${${_pkg_search}_file})
            endif()
            find_file(${_pkg_search}_file
                    NAMES lib${_pkg_search}.so
                    ${_find_opts})
            list(APPEND _libs "${${_pkg_search}_file}")
        else()
            continue()
        endif()
    endforeach()

    set(${_prefix} "${_libs}" PARENT_SCOPE)
endfunction()

#Define native-lib (sources + headers)
include_directories(src/main/cpp/include)
FILE(GLOB_RECURSE SOURCES src/main/cpp/src/*.cpp)
FILE(GLOB_RECURSE HEADERS src/main/cpp/include/*.h)

#Search for pkgconfig external libraries
MESSAGE(STATUS "Compiling for ${ANDROID_ABI}")

set(_ARCH "")
if(${ANDROID_ABI} STREQUAL "arm" OR ${ANDROID_ABI} STREQUAL "armeabi-v7a" OR ${ANDROID_ABI} STREQUAL "armeabi")
    set(_ARCH "arm")
    set(ENV{PKG_CONFIG_PATH} "${CMAKE_PREFIX_PATH_ARM}")
elseif(${ANDROID_ABI} STREQUAL "x86")
    set(_ARCH "x86")
    set(ENV{PKG_CONFIG_PATH} "${CMAKE_PREFIX_PATH_X86}")
endif()

#Check module in PKG
find_package(PkgConfig              REQUIRED)
PKG_CHECK_MODULES(LIB3DS            REQUIRED lib3ds)
PKG_CHECK_MODULES(LIBPNG            REQUIRED libpng16)
PKG_CHECK_MODULES(GLM               REQUIRED glm)
PKG_CHECK_MODULES(SERENO_VTK_PARSER REQUIRED serenoVTKParser)
PKG_CHECK_MODULES(SERENO_MATH       REQUIRED serenoMath)
PKG_CHECK_MODULES(SERENO_SCI_VIS    REQUIRED serenoSciVis)

#Find android internal libraries
find_library(log-lib     log)
find_library(android-lib android)
find_library(egl-lib     EGL)
find_library(gles-lib    GLESv3)
find_library(z-lib       z)

#######################
#Add external libraries
#######################

#3ds lib
findSO(3DS_SO LIB3DS)
add_library(3ds-lib SHARED IMPORTED)
set_target_properties(3ds-lib PROPERTIES
        IMPORTED_LOCATION "${3DS_SO}")

#libpng
findSO(LIBPNG_SO LIBPNG)
MESSAGE(STATUS "${LIBPNG_SO}")
add_library(png-lib SHARED IMPORTED)
set_target_properties(png-lib PROPERTIES
        IMPORTED_LOCATION "${LIBPNG_SO}")

#sereno vtk parser
findSO(SERENO_VTK_PARSER_SO SERENO_VTK_PARSER)
MESSAGE(STATUS ${SERENO_VTK_PARSER_SO})
add_library(sereno_vtk_parser-lib SHARED IMPORTED)
set_target_properties(sereno_vtk_parser-lib PROPERTIES
        IMPORTED_LOCATION "${SERENO_VTK_PARSER_SO}")

#sereno math
findSO(SERENO_MATH_SO SERENO_MATH)
MESSAGE(STATUS ${SERENO_MATH_SO})
add_library(sereno_math-lib SHARED IMPORTED)
set_target_properties(sereno_math-lib PROPERTIES
        IMPORTED_LOCATION "${SERENO_MATH_SO}")

#sereno sciVis
findSO(SERENO_SCI_VIS_SO SERENO_SCI_VIS)
MESSAGE(STATUS ${SERENO_SCI_VIS_SO})
add_library(sereno_sci_vis-lib SHARED IMPORTED)
set_target_properties(sereno_sci_vis-lib PROPERTIES
        IMPORTED_LOCATION "${SERENO_SCI_VIS_SO}")

#OpenMP configuration
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${OpenMP_C_FLAGS}   -g")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -g")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else()
    MESSAGE(ERROR "Could not found OpenMP...")
endif()

#Asan
add_library(asan-rt SHARED IMPORTED)
set_property(TARGET asan-rt PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}/libclang_rt.asan-${_ARCH}-android.so")

#Add our library
add_library(native-lib SHARED ${SOURCES} ${HEADERS})
add_definitions(-DSNAPSHOT)

target_compile_options(native-lib PRIVATE  ${LIB3DS_CFLAGS} ${GLM_CFLAGS} ${SERENO_VTK_PARSER_CFLAGS} ${SERENO_MATH_CFLAGS} ${SERENO_SCI_VIS_CFLAGS} ${OpenMP_CXX_FLAGS})
target_link_libraries(native-lib  ${GLM_LDFLAGS} png-lib 3ds-lib sereno_vtk_parser-lib sereno_math-lib sereno_sci_vis-lib
                      ${z-lib} ${log-lib} ${egl-lib} ${gles-lib} ${android-lib} ${OpenMP_EXE_LINKER_FLAGS} -static-openmp)

#Copy .so
if(ANDROID)
    add_custom_target(COPY_SO ALL)
    add_dependencies(COPY_SO native-lib)
    add_custom_command(TARGET COPY_SO POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SERENO_SCI_VIS_SO}    ${PROJECT_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}/)
    add_custom_command(TARGET COPY_SO POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SERENO_MATH_SO}       ${PROJECT_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}/)
    add_custom_command(TARGET COPY_SO POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SERENO_VTK_PARSER_SO} ${PROJECT_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}/)
    add_custom_command(TARGET COPY_SO POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${3DS_SO}               ${PROJECT_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}/)
    add_custom_command(TARGET COPY_SO POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LIBPNG_SO}            ${PROJECT_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}/)
endif()

#Gdb
include(AndroidNdkGdb)
android_ndk_gdb_enable()
android_ndk_gdb_debuggable(native-lib)

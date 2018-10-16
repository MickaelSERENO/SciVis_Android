cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH 1)

set(PKG_CONFIG_PATH_ARM "" CACHE PATH "Path to pkgconfig files for arm architecture")


#Define native-lib (sources + headers)
include_directories(src/main/cpp/include)
FILE(GLOB_RECURSE SOURCES src/main/cpp/src/*.cpp)
FILE(GLOB_RECURSE HEADERS src/main/cpp/include/*.h)
add_library(native-lib SHARED ${SOURCES} ${HEADERS})

#Search for pkgconfig external libraries
if(${ANDROID_ABI} STREQUAL "arm" OR ${ANDROID_ABI} STREQUAL "armeabi-v7a" OR ${ANDROID_ABI} STREQUAL "armeabi")
    set(ENV{PKG_CONFIG_PATH} "${CMAKE_PREFIX_PATH_ARM}")
endif(${ANDROID_ABI} STREQUAL "arm" OR ${ANDROID_ABI} STREQUAL "armeabi-v7a" OR ${ANDROID_ABI} STREQUAL "armeabi")

find_package(PkgConfig    REQUIRED)
PKG_CHECK_MODULES(LIB3DS  REQUIRED lib3ds)
PKG_CHECK_MODULES(GLM     REQUIRED glm)

#Find android internal libraries
find_library(log-lib     log)
find_library(android-lib android)
find_library(egl-lib     EGL)
find_library(gles-lib    GLESv2)

target_compile_options(native-lib PUBLIC ${LIB3DS_CFLAGS} ${GLM_CFLAGS})
target_link_libraries(native-lib PUBLIC ${log-lib} ${egl-lib} ${gles-lib} ${android-lib}
                                        ${LIB3DS_LDFLAGS} ${GLM_LDFLAGS})

cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(src/main/cpp/include)

FILE(GLOB_RECURSE SOURCES src/main/cpp/src/*.cpp)
FILE(GLOB_RECURSE HEADERS src/main/cpp/include/*.h)

add_library(native-lib SHARED ${SOURCES} ${HEADERS})

find_library(log-lib     log)
find_library(android-lib android)
find_library(egl-lib     EGL)
find_library(gles-lib    GLESv2)

target_link_libraries(native-lib ${log-lib} ${egl-lib} ${gles-lib} ${android-lib})

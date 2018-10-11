cmake_minimum_required(VERSION 3.0.0)

include_directories(src/main/cpp/include)

FILE(GLOB_RECURSE SOURCES src/main/cpp/src/*.cpp)
FILE(GLOB_RECURSE HEADERS src/main/cpp/include/*.h)

add_library(native-lib SHARED ${SOURCES} ${HEADERS})

find_library(log-lib log)

target_link_libraries(native-lib ${log-lib})

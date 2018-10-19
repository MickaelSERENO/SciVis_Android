macro(android_ndk_gdb_enable)
    if(ANDROID)
        # create custom target that depends on the real target so it gets executed afterwards
        add_custom_target(NDK_GDB ALL)

        if(${ARGC})
            set(ANDROID_PROJECT_DIR ${ARGV0})
        else()
            set(ANDROID_PROJECT_DIR ${PROJECT_SOURCE_DIR})
        endif()

        set(NDK_GDB_SOLIB_PATH ${ANDROID_PROJECT_DIR}/obj/local/${ANDROID_NDK_ABI_NAME}/)
        file(MAKE_DIRECTORY ${NDK_GDB_SOLIB_PATH})

        # 1. generate essential Android Makefiles
        file(MAKE_DIRECTORY ${ANDROID_PROJECT_DIR}/jni)
        if(NOT EXISTS ${ANDROID_PROJECT_DIR}/jni/Android.mk)
            file(WRITE ${ANDROID_PROJECT_DIR}/jni/Android.mk "APP_ABI := ${ANDROID_NDK_ABI_NAME}\n")
        endif()
        if(NOT EXISTS ${ANDROID_PROJECT_DIR}/jni/Application.mk)
            file(WRITE ${ANDROID_PROJECT_DIR}/jni/Application.mk "APP_ABI := ${ANDROID_NDK_ABI_NAME}\n")
        endif()

        # 2. generate gdb.setup
        get_directory_property(PROJECT_INCLUDES DIRECTORY ${PROJECT_SOURCE_DIR} INCLUDE_DIRECTORIES)
        string(REGEX REPLACE ";" " " PROJECT_INCLUDES "${PROJECT_INCLUDES}")
        file(WRITE ${CMAKE_SOURCE_DIR}/libs/gdb.setup "set solib-search-path ${NDK_GDB_SOLIB_PATH}\n")
        file(APPEND ${CMAKE_SOURCE_DIR}/libs/gdb.setup "directory ${PROJECT_INCLUDES}\n")

        # 3. copy gdbserver executable
        file(COPY ${ANDROID_NDK}/prebuilt/android-${ANDROID_ARCH_NAME}/gdbserver/gdbserver DESTINATION ${CMAKE_SOURCE_DIR}/libs/)
    endif()
endmacro()

# register a target for remote debugging
# copies the debug version to NDK_GDB_SOLIB_PATH then strips symbols of original
macro(android_ndk_gdb_debuggable TARGET_NAME)
    if(ANDROID)
        get_property(TARGET_LOCATION TARGET ${TARGET_NAME} PROPERTY LOCATION)

        # create custom target that depends on the real target so it gets executed afterwards
        add_dependencies(NDK_GDB ${TARGET_NAME})

        # 4. copy lib to obj
        add_custom_command(TARGET NDK_GDB POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TARGET_LOCATION} ${NDK_GDB_SOLIB_PATH})

        # 5. strip symbols
        add_custom_command(TARGET NDK_GDB POST_BUILD COMMAND ${CMAKE_STRIP} ${TARGET_LOCATION})
    endif()
endmacro()

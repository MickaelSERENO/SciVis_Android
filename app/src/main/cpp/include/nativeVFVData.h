#ifndef NATIVEVFVDATA_INC
#define NATIVEVFVDATA_INC

#include <jni.h>
#include "VFVData.h"

extern "C"
{
    JNIEXPORT jlong JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeCreateMainArgs(JNIEnv *env, jobject instance);
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeAddData(JNIEnv* env, jobject instance, jlong ptr, jstring jDataPath);
    JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeRemoveData(JNIEnv* env, jobject instance, jlong ptr, jstring jDataPath);
}

#endif

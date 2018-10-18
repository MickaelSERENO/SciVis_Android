#include "nativeVFVData.h"

using namespace sereno;

JNIEXPORT jlong JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeCreateMainArgs(JNIEnv *env, jobject instance)
{
    VFVData* data = new VFVData();
    return (jlong)data;
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeDeleteMainArgs(JNIEnv *env, jobject instance, jlong ptr)
{
    VFVData* data = new VFVData();
    delete data;
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeSetCurrentData(JNIEnv* env, jobject instance, jlong ptr, jint dataIdx)
{
    VFVData* data = (VFVData*)ptr;
    data->setCurrentData(dataIdx);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeAddData(JNIEnv* env, jobject instance, jlong ptr, jlong jData)
{
    VFVData* data = (VFVData*)ptr;
    data->addData((FluidDataset*)jData);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeRemoveData(JNIEnv* env, jobject instance, jlong ptr, jint dataIdx)
{
    VFVData* data = (VFVData*)ptr;
    data->removeData(dataIdx);
}

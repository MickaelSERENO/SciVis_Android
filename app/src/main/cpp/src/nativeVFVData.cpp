#include "nativeVFVData.h"
#include <memory>

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

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeAddBinaryDataset(JNIEnv* env, jobject instance, jlong ptr, jlong jData)
{
    VFVData* data = (VFVData*)ptr;
    data->addBinaryData(*((std::shared_ptr<BinaryDataset>*)jData));
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeRemoveData(JNIEnv* env, jobject instance, jlong ptr, jint dataIdx)
{
    VFVData* data = (VFVData*)ptr;
    data->removeData(dataIdx);
}

JNIEXPORT void  JNICALL Java_com_sereno_gl_VFVSurfaceView_nativeOnRangeColorChange(JNIEnv* env, jobject instance, jlong ptr, jfloat min, jfloat max, jint mode)
{
    VFVData* data = (VFVData*)ptr;
    data->onRangeColorChange(min, max, (ColorMode)(mode));
}

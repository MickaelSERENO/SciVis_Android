#ifndef  JNIDATA_INC
#define  JNIDATA_INC

#include <jni.h>

namespace sereno
{
    extern JavaVM* javaVM;
    extern JNIEnv* jniMainThread;

    extern jclass    jBitmapClass;
    extern jmethodID jBitmap_createBitmap;

    extern jclass    jBitmapConfigClass;
    extern jfieldID  jBitmapConfig_ARGB;
    extern jobject   jBitmapConfigARGB;

    extern jclass    jDatasetClass;
    extern jmethodID jDataset_getNbSubDataset;
    extern jmethodID jDataset_getSubDataset;

    extern jclass    jSubDatasetClass;
    extern jmethodID jSubDataset_onRotationEvent;
    extern jmethodID jSubDataset_onSnapshotEvent;
}

extern "C"
{
    jint JNI_OnLoad(JavaVM* vm, void* reserved);
    void JNI_OnUnload(JavaVM *vm, void *reserved);
}

#endif

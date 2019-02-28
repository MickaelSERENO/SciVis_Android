#ifndef  JNIDATA_INC
#define  JNIDATA_INC

#include <jni.h>

namespace sereno
{
    extern JavaVM* javaVM;
    extern JNIEnv* jniMainThread;
    extern jclass    jSurfaceViewClass;
    extern jmethodID jSurfaceView_onRotationEvent;
}

extern "C"
{
    jint JNI_OnLoad(JavaVM* vm, void* reserved);
    void JNI_OnUnload(JavaVM *vm, void *reserved);
}

#endif

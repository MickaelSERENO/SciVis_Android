#include "jniData.h"

namespace sereno
{
    JavaVM*   javaVM                       = NULL;
    JNIEnv*   jniMainThread                = NULL;
    jclass    jSurfaceViewClass            = 0;
    jmethodID jSurfaceView_onRotationEvent = 0;
}

using namespace sereno;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    sereno::javaVM = vm;

    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
        return -1;

    //Load classes
    jclass surfaceCls            = env->FindClass("com/sereno/gl/VFVSurfaceView");
    jSurfaceViewClass            = (jclass)env->NewGlobalRef(surfaceCls);
    env->DeleteLocalRef(surfaceCls);

    //Load methods
    jSurfaceView_onRotationEvent = env->GetMethodID(jSurfaceViewClass, "onRotationEvent", "(JFFF)V");

    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM *vm, void *reserved)
{
    JNIEnv* env;
    if(vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
        return;

    env->DeleteGlobalRef(jSurfaceViewClass);
}
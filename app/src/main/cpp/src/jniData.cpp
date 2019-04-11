#include "jniData.h"

namespace sereno
{
    JavaVM*   javaVM                           = NULL;
    JNIEnv*   jniMainThread                    = NULL;

    jclass    jVFVSurfaceViewClass             = 0;
    jmethodID jVFVSurfaceView_setCurrentAction = 0;
    jmethodID jVFVSurfaceView_getCurrentAction = 0;

    jclass    jBitmapClass                     = 0;
    jmethodID jBitmap_createBitmap             = 0;

    jclass    jBitmapConfigClass               = 0;
    jfieldID  jBitmapConfig_ARGB               = 0;
    jobject   jBitmapConfigARGB                = 0;

    jclass    jDatasetClass                    = 0;
    jmethodID jDataset_getNbSubDataset         = 0;
    jmethodID jDataset_getSubDataset           = 0;

    jclass    jSubDatasetClass                 = 0;
    jmethodID jSubDataset_onRotationEvent      = 0;
    jmethodID jSubDataset_onSnapshotEvent      = 0;
}

using namespace sereno;

jclass getJNIClassGlobalReference(JNIEnv* env, const char* name)
{
    jclass cls = env->FindClass(name);
    jclass res = (jclass)env->NewGlobalRef(cls);
    env->DeleteLocalRef(cls);
    return res;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    sereno::javaVM = vm;

    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
        return -1;

    //Load classes
    jVFVSurfaceViewClass = getJNIClassGlobalReference(env, "com/sereno/gl/VFVSurfaceView");
    jBitmapClass         = getJNIClassGlobalReference(env, "android/graphics/Bitmap");
    jBitmapConfigClass   = getJNIClassGlobalReference(env, "android/graphics/Bitmap$Config");
    jDatasetClass        = getJNIClassGlobalReference(env, "com/sereno/vfv/Data/Dataset");
    jSubDatasetClass     = getJNIClassGlobalReference(env, "com/sereno/vfv/Data/SubDataset");

    //Load methods
    jVFVSurfaceView_getCurrentAction = env->GetMethodID(jVFVSurfaceViewClass, "getCurrentAction", "()I");
    jVFVSurfaceView_setCurrentAction = env->GetMethodID(jVFVSurfaceViewClass, "setCurrentAction", "(I)V");

    jDataset_getNbSubDataset    = env->GetMethodID(jDatasetClass, "getNbSubDataset", "()I");
    jDataset_getSubDataset      = env->GetMethodID(jDatasetClass, "getSubDataset", "(I)Lcom/sereno/vfv/Data/SubDataset;");

    jSubDataset_onRotationEvent = env->GetMethodID(jSubDatasetClass, "onRotationEvent", "(FFF)V");
    jSubDataset_onSnapshotEvent = env->GetMethodID(jSubDatasetClass, "onSnapshotEvent", "(Landroid/graphics/Bitmap;)V");

    //Load fields
    jBitmapConfig_ARGB   = env->GetStaticFieldID(jBitmapConfigClass, "ARGB_8888", "Landroid/graphics/Bitmap$Config;");
    jBitmap_createBitmap = env->GetStaticMethodID(jBitmapClass, "createBitmap", "([IIILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    
    //Load static object
    jobject bmpConfARGB = env->GetStaticObjectField(jBitmapConfigClass, jBitmapConfig_ARGB);
    jBitmapConfigARGB   = env->NewGlobalRef(bmpConfARGB);
    env->DeleteLocalRef(bmpConfARGB);

    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM *vm, void *reserved)
{
    JNIEnv* env;
    if(vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
        return;

    //Delete global references
    env->DeleteGlobalRef(jVFVSurfaceViewClass);
    env->DeleteGlobalRef(jBitmapClass);
    env->DeleteGlobalRef(jBitmapConfigClass);
    env->DeleteGlobalRef(jDatasetClass);
    env->DeleteGlobalRef(jSubDatasetClass);
    env->DeleteGlobalRef(jBitmapConfigARGB);
}
